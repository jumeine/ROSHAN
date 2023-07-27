import torch
import torch.nn as nn
import numpy as np
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

    def __init__(self, scan_size, action_std, input_style, lr, betas, gamma, _lambda, K_epochs, eps_clip, logger, advantages_func, restore=False, ckpt=None):
        self.lr = lr
        self.betas = betas
        self.gamma = gamma
        self._lambda = _lambda
        self.logger = logger
        self.eps_clip = eps_clip
        self.K_epochs = K_epochs

        self.advantages_func = advantages_func

        # current policy
        self.policy = ActorCritic(action_std, scan_size, input_style, logger)
        if restore:
            pretained_model = torch.load(ckpt, map_location=lambda storage, loc: storage)
            self.policy.load_state_dict(pretained_model)

        self.optimizer_a = torch.optim.Adam(self.policy.actor.parameters(), lr=lr, betas=betas, eps=1e-5)
        self.optimizer_c = torch.optim.Adam(self.policy.critic.parameters(), lr=lr, betas=betas, eps=1e-5)
        #self.optimizer = torch.optim.Adam(self.policy.ac.parameters(), lr=lr, betas=betas, eps=1e-5)

        # old policy: initialize old policy with current policy's parameter
        self.old_policy = ActorCritic(action_std, scan_size, input_style, logger)
        self.old_policy.load_state_dict(self.policy.state_dict())

        self.MSE_loss = nn.MSELoss()
        self.running_reward_std = RunningMeanStd()

    def select_action(self, observations):
        return self.old_policy.act(observations)

    def select_action_certain(self, observations):
        return self.old_policy.act_certain(observations)

    def saveCurrentWeights(self, ckpt_folder, env_name):
        print('Saving current weights to ' + ckpt_folder + '/' + env_name + '_current.pth')
        torch.save(self.policy.state_dict(), ckpt_folder + '/PPO_continuous_{}_current.pth'.format(env_name))

    def get_advantages(self, values, masks, rewards):
        """
        Computes the advantages of the given rewards and values.

        :param values: The values of the states.
        :param masks: The masks of the states.
        :param rewards: The rewards of the states.
        :return: The advantages of the states.
        """
        returns = []
        gae = 0
        a = [ai.unsqueeze(0) for ai in values]
        a.append(torch.tensor([0.], requires_grad=True).to(device))
        values = torch.cat(a).squeeze(0)
        for i in reversed(range(len(rewards))):
            delta = rewards[i] + self.gamma * values[i + 1] * masks[i] - values[i]
            gae = delta + self.gamma * 0.95 * masks[i] * gae
            returns.insert(0, gae + values[i])

        returns = torch.FloatTensor(returns).to(device)
        adv = returns - values[:-1]
        return returns, (adv - adv.mean()) / (adv.std() + 1e-10)

    def update(self, memory, batches):
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
        :param batches: The number of batches to divide the memory into.
        """
        # computes the discounted reward for every robots in memory
        rewards = []
        masks = []

        for robotmemory in memory.swarmMemory:
            _rewards = []
            _masks = []
            for reward, is_terminal in zip(reversed(robotmemory.rewards), reversed(robotmemory.is_terminals)):
                _masks.insert(0, 1 - is_terminal)
                _rewards.insert(0, reward)
            rewards.append(_rewards)
            masks.append(_masks)

        # flatten the rewards
        rewards = [item for sublist in rewards for item in sublist]
        masks = [item for sublist in masks for item in sublist]

        # Normalize rewards
        self.running_reward_std.update(np.array(rewards))
        rewards = np.clip(np.array(rewards) / self.running_reward_std.get_std(), -10, 10)
        rewards = torch.tensor(rewards).type(torch.float32)

        masks = torch.tensor(masks)

        # convert list to tensor
        old_states = memory.getObservationOfAllRobots()
        laser, orientation, distance, velocity = old_states
        old_actions = torch.stack(memory.getActionsOfAllRobots())
        old_logprobs = torch.stack(memory.getLogProbsOfAllRobots())

        # TODO randomize the order of experiences that it DOESNT interfer with GAE calculation

        # Train policy for K epochs: sampling and updating
        for rewards_minibatch, old_laser_minibatch, old_orientation_minibatch, old_distance_minibatch, \
                old_velocity_minibatch, old_actions_minibatch, old_logprobs_minibatch, mask_minibatch in \
                zip(torch.tensor_split(rewards, batches), torch.tensor_split(laser, batches),
                    torch.tensor_split(orientation, batches), torch.tensor_split(distance, batches),
                    torch.tensor_split(velocity, batches), torch.tensor_split(old_actions, batches),
                    torch.tensor_split(old_logprobs, batches), torch.tensor_split(masks, batches)):

            old_states_minibatch = [old_laser_minibatch.detach().clone().to(device),
                                    old_orientation_minibatch.detach().clone().to(device),
                                    old_distance_minibatch.detach().clone().to(device),
                                    old_velocity_minibatch.detach().clone().to(device)]
            # Advantages
            old_actions_minibatch = old_actions_minibatch.detach().clone().to(device)
            old_logprobs_minibatch = old_logprobs_minibatch.detach().clone().to(device)
            mask_minibatch = mask_minibatch.to(device)
            _, values_, _ = self.policy.evaluate(old_states_minibatch, old_actions_minibatch)
            #returns, advantages = self.get_advantages(values_.detach(), mask_minibatch, rewards_minibatch)
            returns, advantages = self.advantages_func(self.gamma, self._lambda, values_, mask_minibatch, rewards_minibatch.to(device))

            for _ in range(self.K_epochs):
                # Evaluate old actions and values using current policy
                logprobs, values, dist_entropy = self.policy.evaluate(old_states_minibatch, old_actions_minibatch)

                # Importance ratio: p/q
                ratios = torch.exp(logprobs - old_logprobs_minibatch)

                # Advantages
                #returns, advantages = self.get_advantages(state_values.detach(), mask_minibatch, rewards_minibatch)
                #advantages = rewards_minibatch - state_values.detach()
                #advantages = (advantages - advantages.mean()) / (advantages.std() + 1e-10)

                # Actor loss using Surrogate loss
                surr1 = ratios * advantages
                surr2 = torch.clamp(ratios, 1 - self.eps_clip, 1 + self.eps_clip) * advantages
                entropy = 0.001 * dist_entropy
                actor_loss = - torch.min(surr1, surr2).type(torch.float32)

                # TODO CLIP VALUE LOSS ? Probably not necessary as according to:
                # https://iclr-blog-track.github.io/2022/03/25/ppo-implementation-details/
                critic_loss_ = 0.005 * self.MSE_loss(returns, values)
                critic_loss = critic_loss_ - entropy

                # Total loss
                loss = actor_loss + critic_loss
                self.logger.add_loss(loss.detach().mean().item(), entropy=entropy.detach().mean().item(), critic_loss=critic_loss.detach().mean().item(), actor_loss=actor_loss.detach().mean().item())
                # Backward gradients
                self.optimizer_a.zero_grad()
                actor_loss.mean().backward(retain_graph=True)
                # Global gradient norm clipping https://vitalab.github.io/article/2020/01/14/Implementation_Matters.html
                torch.nn.utils.clip_grad_norm_(self.policy.actor.parameters(), max_norm=0.5)
                self.optimizer_a.step()
                self.optimizer_a.zero_grad()

                self.optimizer_c.zero_grad()
                critic_loss.mean().backward()
                # Global gradient norm clipping https://vitalab.github.io/article/2020/01/14/Implementation_Matters.html
                torch.nn.utils.clip_grad_norm_(self.policy.critic.parameters(), max_norm=0.5)
                self.optimizer_c.step()
                # self.optimizer.zero_grad()
                # loss.mean().backward()
                # # Global gradient norm clipping https://vitalab.github.io/article/2020/01/14/Implementation_Matters.html
                # torch.nn.utils.clip_grad_norm_(self.policy.ac.parameters(), max_norm=0.5)
                # self.optimizer.step()

        # Copy new weights to old_policy
        self.old_policy.actor.load_state_dict(self.policy.actor.state_dict())
        self.old_policy.critic.load_state_dict(self.policy.critic.state_dict())
        # self.old_policy.ac.load_state_dict(self.policy.ac.state_dict())