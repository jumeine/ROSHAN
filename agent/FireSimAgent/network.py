import torch.nn as nn
import torch.nn.functional as F
from torch.distributions import MultivariateNormal
import torch
import pdb

torch.autograd.set_detect_anomaly(True)

from utils import initialize_hidden_weights, initialize_output_weights

device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")


class Inputspace(nn.Module):

    def __init__(self, vision_range):
        """
        A PyTorch Module that represents the input space of a neural network.
        """
        super(Inputspace, self).__init__()

        # 2D Convolutional Layers for terrain and fire status
        self.terrain_conv1 = nn.Conv2d(in_channels=4, out_channels=2, kernel_size=3, stride=1)
        initialize_hidden_weights(self.terrain_conv1)
        in_f = self.get_in_features(h_in=vision_range, kernel_size=3, stride=1)
        features_terrain = (int(in_f * in_f)) * 2 #2 is the number of output channels

        self.fire_conv1 = nn.Conv2d(in_channels=4, out_channels=2, kernel_size=3, stride=1)
        initialize_hidden_weights(self.fire_conv1)
        in_f = self.get_in_features(h_in=vision_range, kernel_size=3, stride=1)
        features_fire = (int(in_f * in_f)) * 2 #32 is the number of output channels

        self.flatten = nn.Flatten()

        ori_out_features = 32
        vel_out_features = 32

        self.ori_dense = nn.Linear(in_features=2, out_features=ori_out_features)
        initialize_hidden_weights(self.ori_dense)
        self.vel_dense = nn.Linear(in_features=2, out_features=vel_out_features)
        initialize_hidden_weights(self.vel_dense)

        # four is the number of timeframes TODO make this dynamic
        terrain_out_features = 192
        fire_out_features = 192
        input_features = terrain_out_features + fire_out_features + (ori_out_features + vel_out_features) * 4

        self.terrain_flat = nn.Linear(in_features=features_terrain, out_features=terrain_out_features)
        initialize_hidden_weights(self.terrain_flat)

        self.fire_flat = nn.Linear(in_features=features_fire, out_features=fire_out_features)
        initialize_hidden_weights(self.fire_flat)


        self.input_dense = nn.Linear(in_features=input_features, out_features=256)
        initialize_hidden_weights(self.input_dense)

    def get_in_features(self, h_in, padding=0, dilation=1, kernel_size=0, stride=1):
        return (h_in - kernel_size + 2*padding) // stride + 1

    # def get_in_features(self, h_in, padding=0, dilation=1, kernel_size=0, stride=1):
    #     return (((h_in + 2 * padding - dilation * (kernel_size - 1) - 1) / stride) + 1)

    def forward(self, terrain, fire_status, orientation, velocity):
        terrain = F.relu(self.terrain_conv1(terrain))
        terrain = self.flatten(terrain)
        terrain = F.relu(self.terrain_flat(terrain))

        fire_status = F.relu(self.fire_conv1(fire_status))
        fire_status = self.flatten(fire_status)
        fire_status = F.relu(self.fire_flat(fire_status))

        orientation = F.relu(self.ori_dense(orientation))
        orientation = self.flatten(orientation)

        velocity = F.relu(self.vel_dense(velocity))
        velocity = self.flatten(velocity)

        concated_input = torch.cat((terrain, fire_status, orientation, velocity), dim=1)
        input_dense = F.relu(self.input_dense(concated_input))

        return input_dense



class Actor(nn.Module):
    """
    A PyTorch Module that represents the actor network of a PPO agent.

    This module takes in four inputs: a lidar scan, orientation to goal, distance to goal, and velocity.
    It then applies convolutional and dense layers to each input separately and concatenates the outputs
    to produce a flattened feature vector that can be fed into a downstream neural network.

    :param scan_size: The number of lidar scans in the input lidar scan.
    """
    def __init__(self, vision_range):
        super(Actor, self).__init__()
        self.Inputspace = Inputspace(vision_range)

        # Mu
        self.mu = nn.Linear(in_features=256, out_features=2)
        initialize_output_weights(self.mu, 'actor')
        # Logstd
        self.log_std = nn.Parameter(torch.zeros(2, ))

    def forward(self, terrain, fire_status, orientation, velocity):
        x = self.Inputspace(terrain, fire_status, orientation, velocity)
        mu = torch.tanh(self.mu(x))
        std = torch.exp(self.log_std)
        var = torch.pow(std, 2)

        return mu, var


class Critic(nn.Module):
    """
    A PyTorch Module that represents the critic network of a PPO agent.

    This module takes in four inputs: a lidar scan, orientation to goal, distance to goal, and velocity.
    It then applies convolutional and dense layers to each input separately and concatenates the outputs
    to produce a flattened feature vector that can be fed into a downstream neural network.

    :param scan_size: The number of lidar scans in the input lidar scan.
    """
    def __init__(self, vision_range):
        super(Critic, self).__init__()
        self.Inputspace = self.Inputspace = Inputspace(vision_range)
        # Value
        self.value = nn.Linear(in_features=256, out_features=1)
        initialize_output_weights(self.value, 'critic')

    def forward(self, terrain, fire_status, orientation, velocity):
        x = self.Inputspace(terrain, fire_status, orientation, velocity)
        value = F.relu(self.value(x.detach().clone()))
        return value


class ActorCritic(nn.Module):
    """
    A PyTorch Module that represents the actor-critic network of a PPO agent.
    """
    def __init__(self, vision_range):
        super(ActorCritic, self).__init__()
        self.actor_cnt = 0
        self.critic_cnt = 0
        self.device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")
        self.actor = Actor(vision_range).to(device)
        self.critic = Critic(vision_range).to(device)

        # TODO statische var testen
        #self.logstds_param = nn.Parameter(torch.full((n_actions,), 0.1))
        #self.action_var = torch.full((action_dim, ), action_std * action_std).to(device)

    def act(self, state):
        """
        Returns an action sampled from the actor's distribution and the log probability of that action.

        :param states: A tuple of the current lidar scan, orientation to goal, distance to goal, and velocity.
        :return: A tuple of the sampled action and the log probability of that action.
        """
        terrain, fire_status, orientation, velocity = state
        terrain = torch.tensor(terrain, dtype=torch.float32)
        fire_status = torch.tensor(fire_status, dtype=torch.float32)
        orientation = torch.tensor(orientation, dtype=torch.float32)
        velocity = torch.tensor(velocity, dtype=torch.float32)

        # TODO: check if normalization of states is necessary
        # was suggested in: Implementation_Matters in Deep RL: A Case Study on PPO and TRPO
        action_mean, action_var = self.actor(terrain.to(device), fire_status.to(device), orientation.to(device), velocity.to(device))
        #_, action_mean, action_var = self.ac(laser, orientation, distance, velocity)

        action_mean = action_mean.to('cpu')
        action_var = action_var.to('cpu')

        cov_mat = torch.diag(action_var)
        dist = MultivariateNormal(action_mean, cov_mat)
        ## logging of actions
        #self.logger.add_actor_output(action_mean.mean(0)[0].item(), action_mean.mean(0)[1].item(), action_var[0].item(), action_var[1].item())

        action = dist.sample()
        action = torch.clip(action, -1, 1)
        action_logprob = dist.log_prob(action)

        return action.detach().numpy(), action_logprob.detach().numpy()

    def act_certain(self, state):
        """
        Returns an action from the actor's distribution without sampling.

        :param states: A tuple of the current lidar scan, orientation to goal, distance to goal, and velocity.
        :return: The action from the actor's distribution.
        """
        terrain, fire_status, orientation, velocity = state
        action, _ = self.actor(terrain.to(device), fire_status.to(device), orientation.to(device), velocity.to(device))

        return action.to('cpu')

    def evaluate(self, state, action):
        """
        Returns the log probability of the given action, the value of the given state, and the entropy of the actor's
        distribution.

        :param state: A tuple of the current lidar scan, orientation to goal, distance to goal, and velocity.
        :param action: The action to evaluate.
        :return: A tuple of the log probability of the given action, the value of the given state, and the entropy of the
        actor's distribution.
        """
        terrain, fire_status, orientation, velocity = state
        state_value = self.critic(terrain.to(device), fire_status.to(device), orientation.to(device), velocity.to(device))

        action_mean, action_var = self.actor(terrain.to(device), fire_status.to(device), orientation.to(device), velocity.to(device))

        cov_mat = torch.diag(action_var)
        dist = MultivariateNormal(action_mean, cov_mat)
        action_logprobs = dist.log_prob(action)
        dist_entropy = dist.entropy()

        return action_logprobs.to(device), torch.squeeze(state_value), dist_entropy.to(device)
