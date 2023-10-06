import cProfile
import sys
from typing import List, Tuple, Deque

from ppo import PPO
from td3 import TD3

sys.path.insert(0, '../../cmake-build-debug')


class Agent:
    def __init__(self, algorithm: str = 'ppo', action_dim=(1, 3)):
        self.algorithm_name = algorithm
        if algorithm == 'ppo':
            self.algorithm = PPO(vision_range=21, lr=0.0001, betas=(0.9, 0.999), gamma=0.99, _lambda=0.9, K_epochs=4, eps_clip=0.2)
            print("PPO agent initialized")
        elif algorithm == 'td3':
            self.algorithm = TD3(action_dim=action_dim, vision_range=21, discount=0.99, tau=0.005, policy_noise=0.2, noise_clip=0.5, policy_freq=2)
            print("TD3 agent initialized")

    def should_train(self, memory, horizon, t):
        if self.algorithm_name == 'ppo':
            return memory.size > horizon
        elif self.algorithm_name == 'td3':
            if t >= 1000:
                return t % 1000 == 0
            else:
                return False

    def update(self, memory, batch_size):
        self.algorithm.update(memory, batch_size)

    def act(self, observations, t):
        if self.algorithm_name == 'td3':
            if t < 5000:
                return self.algorithm.random_action()

        return self.algorithm.select_action(observations)


