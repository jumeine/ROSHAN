import time
import sys
from typing import List, Tuple, Deque

from ppo import PPO

sys.path.insert(0, '../../cmake-build-debug')

class Agent():
    def __init__(self):
        self.ppo = PPO(vision_range=21, lr=0.0001, betas=(0.9, 0.999), gamma=0.99, _lambda=0.9, K_epochs=4, eps_clip=0.2)
        print("Agent initialized")

    def update(self, observations, actions, rewards, next_observations, dones):
        pass

    def act(self, observations):
        return self.ppo.select_action(observations)

    def wait(self):
        """
        Agent waits for a certain amount of time
        :return:
        """
        i = 0
        while i < 5:
            print("Waiting for ", i, " seconds")
            time.sleep(1)
            i += 1


