import torch
import torch.nn as nn
import warnings
import numpy as np
from copy import deepcopy
from network import ActorCritic
from utils import RunningMeanStd

device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")


class PPO:
    """
    This class represents the PPO Algorithm. It is used to train an actor-critic network.

    :param scan_size: The number of lidar scans in the input lidar scan.
    :param action_std: The standard deviation of the action distribution.
    :param input_style: The style of the input to the network.
    :param lr: The learning rate of the network.
    :param betas: The betas of the Adam optimizer.
    :param gamma: The discount factor.
    :param K_epochs: The number of epochs to train the network.
    :param eps_clip: The epsilon value for clipping.
    :param logger: The logger to log data to.
    :param restore: Whether to restore the network from a checkpoint.
    :param ckpt: The checkpoint to restore from.
    """

    def __init__(self, vision_range, lr, betas, gamma, _lambda, K_epochs, eps_clip, restore=False, ckpt=None):
        self.lr = lr
        self.betas = betas
        self.gamma = gamma
        self._lambda = _lambda
        self.eps_clip = eps_clip
        self.K_epochs = K_epochs

        # current policy
        self.policy = ActorCritic(vision_range=vision_range)
        if restore:
            pretained_model = torch.load(ckpt, map_location=lambda storage, loc: storage)
            self.policy.load_state_dict(pretained_model)

        self.optimizer_a = torch.optim.Adam(self.policy.actor.parameters(), lr=lr, betas=betas, eps=1e-5)
        self.optimizer_c = torch.optim.Adam(self.policy.critic.parameters(), lr=lr, betas=betas, eps=1e-5)
        #self.optimizer = torch.optim.Adam(self.policy.ac.parameters(), lr=lr, betas=betas, eps=1e-5)

        # old policy: initialize old policy with current policy's parameter
        self.old_policy = ActorCritic(vision_range=vision_range)
        self.old_policy.load_state_dict(self.policy.state_dict())

        self.MSE_loss = nn.MSELoss()
        self.running_reward_std = RunningMeanStd()

    def select_action(self, observations):
        return self.policy.act(observations)

    def select_action_certain(self, observations):
        return self.policy.act_certain(observations)

    def saveCurrentWeights(self, ckpt_folder, env_name):
        print('Saving current weights to ' + ckpt_folder + '/' + env_name + '_current.pth')
        torch.save(self.policy.state_dict(), ckpt_folder + '/PPO_continuous_{}_current.pth'.format(env_name))

    def calculate_returns(self, rewards, normalize=False):

        returns = []
        return_ = 0

        for r in reversed(rewards):
            return_ = r + return_ * self.gamma
            returns.insert(0, return_)

        returns = torch.tensor(returns, dtype=torch.float32)

        if normalize:
            returns = (returns - returns.mean()) / (returns.std() + 1e-8)

        return returns.detach().to(device)

    def get_advantages(self, values, masks, rewards):
        """
        Computes the advantages of the given rewards and values.

        :param values: The values of the states.
        :param masks: The masks of the states.
        :param rewards: The rewards of the states.
        :return: The advantages of the states.
        """
        advantages = []
        gae = 0

        for i in reversed(range(len(rewards))):
            delta = rewards[i] - values[i]
            if masks[i] == 1:
                delta = delta + self.gamma * values[i + 1]
            gae = delta + self.gamma * self._lambda * masks[i] * gae
            advantages.insert(0, gae)

        advantages = torch.FloatTensor(advantages).to(device)
        norm_adv = (advantages - advantages.mean()) / (advantages.std() + 1e-10)

        return norm_adv

    def update(self, memory, batch_size, logger):
        """
        This function implements the update step of the Proximal Policy Optimization (PPO) algorithm for a swarm of
        robots. It takes in the memory buffer containing the experiences of the swarm, as well as the number of batches
        to divide the experiences into for training. The function first computes the discounted rewards for each robot
        in the swarm and normalizes them. It then flattens the rewards and masks and converts them to PyTorch tensors.
        Next, the function retrieves the observations, actions, and log probabilities from the memory buffer and divides
        them into minibatches for training. For each minibatch, it calculates the advantages using the generalized
        advantage estimator (GAE) and trains the policy for K epochs using the surrogate loss function. The function
        then updates the weights of the actor and critic networks using the optimizer.
        Finally, the function copies the updated weights to the old policy for future use in the next update step.

        :param memory: The memory to update the network with.
        :param batch_size: The size of batches.
        """

        if batch_size > memory.size:
            warnings.warn("Batch size is larger than memory capacity. Setting batch size to memory capacity.")
            batch_size = memory.size

        if batch_size == 1:
            raise ValueError("Batch size must be greater than 1.")

        memory.build_masks()
        log_rewards = []
        log_values = []
        # Normalize rewards
        # self.running_reward_std.update(np.array(rewards))
        # rewards = np.clip(np.array(rewards) / self.running_reward_std.get_std(), -10, 10)
        # rewards = torch.tensor(rewards).type(torch.float32)

        # TODO randomize the order of experiences that it DOESNT interfer with GAE calculation
        while memory.has_batches():
            states, actions, logprobs, rewards, masks = memory.next_batch(batch_size=batch_size)

            old_states = tuple(state.detach().to(device) for state in states)
            old_actions = actions.detach().to(device)
            old_logprobs = logprobs.detach().to(device)
            old_rewards = rewards.detach().to(device) #/ 10
            log_rewards.append(old_rewards.mean().item())
            # Advantages
            _, values_, _ = self.policy.evaluate(old_states, old_actions)
            log_values.append(values_.detach().mean().item())
            if masks[-1] == 1:
                last_state = (old_states[0][-1].unsqueeze(0), old_states[1][-1].unsqueeze(0), old_states[2][-1].unsqueeze(0), old_states[3][-1].unsqueeze(0), old_states[4][-1].unsqueeze(0))
                bootstrapped_value = self.policy.critic(last_state).detach()
                values_ = torch.cat((values_, bootstrapped_value[0]), dim=0)
            returns = self.calculate_returns(old_rewards, normalize=True)
            advantages = self.get_advantages(values_.detach(), masks, old_rewards)
            # returns, advantages = self.get_advantages(values_.detach(), masks.detach(), old_rewards)

            # Train policy for K epochs: sampling and updating
            for _ in range(self.K_epochs):
                # Evaluate old actions and values using current policy
                logprobs, values, dist_entropy = self.policy.evaluate(old_states, old_actions)

                # Importance ratio: p/q
                ratios = torch.exp(logprobs - old_logprobs)

                # Actor loss using Surrogate loss
                surr1 = ratios * advantages
                surr2 = torch.clamp(ratios, 1 - self.eps_clip, 1 + self.eps_clip) * advantages
                entropy = 0.001 * dist_entropy
                actor_loss = (-torch.min(surr1, surr2).type(torch.float32)).mean()

                # TODO CLIP VALUE LOSS ? Probably not necessary as according to:
                # https://iclr-blog-track.github.io/2022/03/25/ppo-implementation-details/
                critic_loss_ = 0.005 * self.MSE_loss(returns, values)
                critic_loss = critic_loss_ #- entropy.mean()
                # Total loss
                # loss = actor_loss + critic_loss
                # self.logger.add_loss(loss.detach().mean().item(), entropy=entropy.detach().mean().item(), critic_loss=critic_loss.detach().mean().item(), actor_loss=actor_loss.detach().mean().item())

                # Sanity checks
                if torch.isnan(critic_loss).any():
                    print(entropy.mean())
                    print(returns)
                    print(values)
                assert not torch.isnan(actor_loss).any()

                assert not torch.isinf(critic_loss).any()
                assert not torch.isinf(actor_loss).any()
                # Backward gradients
                self.optimizer_a.zero_grad()
                actor_loss.backward(retain_graph=True)
                # Global gradient norm clipping https://vitalab.github.io/article/2020/01/14/Implementation_Matters.html
                torch.nn.utils.clip_grad_norm_(self.policy.actor.parameters(), max_norm=0.5)
                self.optimizer_a.step()

                self.optimizer_c.zero_grad()
                critic_loss.backward()
                # Global gradient norm clipping https://vitalab.github.io/article/2020/01/14/Implementation_Matters.html
                torch.nn.utils.clip_grad_norm_(self.policy.critic.parameters(), max_norm=0.5)
                self.optimizer_c.step()
                # # Global gradient norm clipping https://vitalab.github.io/article/2020/01/14/Implementation_Matters.html
                # torch.nn.utils.clip_grad_norm_(self.policy.ac.parameters(), max_norm=0.5)

        logger.add_reward([np.array(log_rewards).mean()])
        logger.add_value([np.array(log_values).mean()])

        # Save current weights if the mean reward is higher than the best reward so far
        if logger.better_reward():
            print("Saving best weights with reward {}".format(logger.reward_best))
            torch.save(self.old_policy.state_dict(), 'best.pth')

        # Copy new weights to old_policy
        self.old_policy.actor.load_state_dict(deepcopy(self.policy.actor.state_dict()))
        self.old_policy.critic.load_state_dict(deepcopy(self.policy.critic.state_dict()))

        #Clear memory
        memory.clear_memory()
