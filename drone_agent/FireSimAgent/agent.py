import cProfile
import sys
from typing import List, Tuple, Deque

from ppo import PPO

sys.path.insert(0, '../../cmake-build-debug')


class Agent:
    def __init__(self):
        self.ppo = PPO(vision_range=21, lr=0.0001, betas=(0.9, 0.999), gamma=0.99, _lambda=0.9, K_epochs=4, eps_clip=0.2)
        print("Agent initialized")

    def update(self, memory):
        self.ppo.update(memory, 1)

    def act(self, observations):
        # profiler = cProfile.Profile()
        # netout = profiler.runcall(self.ppo.select_action, observations)
        # profiler.print_stats(sort='time')
        # return netout
        return self.ppo.select_action(observations)


