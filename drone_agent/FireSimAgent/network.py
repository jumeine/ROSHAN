import numpy as np
import torch.nn as nn
import torch.nn.functional as F
from torch.distributions import MultivariateNormal, Bernoulli
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

        # Initialize Adaptive Pooling layers
        # self.terrain_adaptive_pool = nn.AdaptiveAvgPool2d((1, 1))
        # self.fire_adaptive_pool = nn.AdaptiveAvgPool2d((1, 1))
        dim_x = 100
        dim_y = 100
        self.map_adaptive_pool = nn.AdaptiveAvgPool2d((dim_y, dim_x))

        # 2D Convolutional Layers for terrain, fire status and map
        self.terrain_conv1 = nn.Conv2d(in_channels=4, out_channels=2, kernel_size=3, stride=1)
        initialize_hidden_weights(self.terrain_conv1)
        in_f = self.get_in_features(h_in=vision_range, kernel_size=3, stride=1)
        features_terrain = (int(in_f * in_f)) * 2 # 2 is the number of output channels

        self.fire_conv1 = nn.Conv2d(in_channels=4, out_channels=2, kernel_size=3, stride=1)
        initialize_hidden_weights(self.fire_conv1)
        in_f = self.get_in_features(h_in=vision_range, kernel_size=3, stride=1)
        features_fire = (int(in_f * in_f)) * 2 # 2 is the number of output channels

        self.map_conv1 = nn.Conv2d(in_channels=4, out_channels=2, kernel_size=3, stride=1)
        initialize_hidden_weights(self.map_conv1)
        features_map = (int(dim_x * dim_y)) * 2 # 2 is the number of output channels

        self.flatten = nn.Flatten()

        vel_out_features = 32
        self.vel_dense = nn.Linear(in_features=2, out_features=vel_out_features)
        initialize_hidden_weights(self.vel_dense)

        position_out_features = 16
        self.position_dense = nn.Linear(in_features=2, out_features=position_out_features)
        initialize_hidden_weights(self.position_dense)

        # four is the number of timeframes TODO make this dynamic
        terrain_out_features = 64
        fire_out_features = 64
        map_out_features = 64
        input_features = terrain_out_features + fire_out_features + map_out_features + (position_out_features + vel_out_features) * 4

        self.terrain_flat = nn.Linear(in_features=features_terrain, out_features=terrain_out_features)
        initialize_hidden_weights(self.terrain_flat)

        self.fire_flat = nn.Linear(in_features=features_fire, out_features=fire_out_features)
        initialize_hidden_weights(self.fire_flat)

        self.map_flat = nn.Linear(in_features=features_map, out_features=map_out_features)
        initialize_hidden_weights(self.map_flat)

        self.input_dense = nn.Linear(in_features=input_features, out_features=256)
        initialize_hidden_weights(self.input_dense)

    def get_in_features(self, h_in, padding=0, dilation=1, kernel_size=0, stride=1):
        return (h_in - kernel_size + 2 * padding) // stride + 1

    # def get_in_features(self, h_in, padding=0, dilation=1, kernel_size=0, stride=1):
    #     return (((h_in + 2 * padding - dilation * (kernel_size - 1) - 1) / stride) + 1)

    def forward(self, terrain, fire_status, velocity, maps, position):
        terrain = F.relu(self.terrain_conv1(terrain))
        terrain = self.flatten(terrain)
        terrain = F.relu(self.terrain_flat(terrain))

        fire_status = F.relu(self.fire_conv1(fire_status))
        fire_status = self.flatten(fire_status)
        fire_status = F.relu(self.fire_flat(fire_status))

        maps = F.relu(self.map_conv1(maps))
        maps = self.map_adaptive_pool(maps)
        maps = self.flatten(maps)
        maps = F.relu(self.map_flat(maps))

        position = F.relu(self.position_dense(position))
        position = self.flatten(position)

        velocity = F.relu(self.vel_dense(velocity))
        velocity = self.flatten(velocity)

        concated_input = torch.cat((terrain, fire_status, velocity, maps, position), dim=1)
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
        self.water_emit = nn.Linear(in_features=256, out_features=1)
        initialize_output_weights(self.mu, 'actor')

        # Logstd
        self.log_std = nn.Parameter(torch.zeros(3, ))

    def forward(self, terrain, fire_status, velocity, maps, position):
        x = self.Inputspace(terrain, fire_status, velocity, maps, position)
        mu = torch.tanh(self.mu(x))
        water_emit_value = torch.sigmoid(self.water_emit(x))
        actions = torch.cat((mu, water_emit_value), dim=1)
        std = torch.exp(self.log_std)
        var = torch.pow(std, 2)

        return actions, var


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

    def forward(self, terrain, fire_status, velocity, maps, position):
        x = self.Inputspace(terrain, fire_status, velocity, maps, position)
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
        terrain, fire_status, velocity, maps, position = state
        terrain = torch.tensor(terrain, dtype=torch.float32)
        fire_status = torch.tensor(fire_status, dtype=torch.float32)
        velocity = torch.tensor(velocity, dtype=torch.float32)
        maps = torch.tensor(maps, dtype=torch.float32)
        position = torch.tensor(position, dtype=torch.float32)

        # TODO: check if normalization of states is necessary
        # was suggested in: Implementation_Matters in Deep RL: A Case Study on PPO and TRPO
        action_mean, action_var = self.actor(terrain.to(device), fire_status.to(device), velocity.to(device), maps.to(device), position.to(device))

        action_mean_velocity = action_mean[:, :2].to('cpu')
        action_var_velocity = action_var[:2].to('cpu')
        action_mean_water_emit = action_mean[:, 2].to('cpu')

        cov_mat = torch.diag(action_var_velocity)
        dist_velocity = MultivariateNormal(action_mean_velocity, cov_mat)
        dist_water = Bernoulli(action_mean_water_emit)
        ## logging of actions
        #self.logger.add_actor_output(action_mean.mean(0)[0].item(), action_mean.mean(0)[1].item(), action_var[0].item(), action_var[1].item())

        action_velocity = dist_velocity.sample()
        action_velocity = torch.clip(action_velocity, -1, 1)
        action_water = dist_water.sample().view(1, -1)


        action_logprob_velocity = dist_velocity.log_prob(action_velocity)
        action_logprob_water = dist_water.log_prob(action_water)
        action = torch.cat([action_velocity, action_water], dim=1)
        combined_logprob = action_logprob_velocity + action_logprob_water

        return action.detach().numpy(), combined_logprob.detach().numpy()

    def act_certain(self, state):
        """
        Returns an action from the actor's distribution without sampling.

        :param states: A tuple of the current lidar scan, orientation to goal, distance to goal, and velocity.
        :return: The action from the actor's distribution.
        """
        terrain, fire_status, velocity, maps, position = state
        action, _ = self.actor(terrain.to(device), fire_status.to(device), velocity.to(device), maps.to(device), position.to(device))

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
        terrain, fire_status, velocity, maps, position = state
        state_value = self.critic(terrain.to(device), fire_status.to(device), velocity.to(device), maps.to(device), position.to(device))

        action_mean, action_var = self.actor(terrain.to(device), fire_status.to(device), velocity.to(device), maps.to(device), position.to(device))

        action_mean_velocity = action_mean[:, :2]
        action_var_velocity = action_var[:2]
        action_mean_water_emit = action_mean[:, 2]

        cov_mat = torch.diag(action_var_velocity)
        dist_velocity = MultivariateNormal(action_mean_velocity, cov_mat)
        dist_water = Bernoulli(action_mean_water_emit)

        action_logprob_velocity = dist_velocity.log_prob(action[:, :2])
        action_logprob_water = dist_water.log_prob(action[:, 2].view(1, -1))
        combined_logprob = action_logprob_velocity + action_logprob_water

        dist_entropy_velocity = dist_velocity.entropy()
        dist_entropy_water = dist_water.entropy()
        combined_entropy = dist_entropy_velocity + dist_entropy_water

        return combined_logprob.to(device), torch.squeeze(state_value), combined_entropy.to(device)
