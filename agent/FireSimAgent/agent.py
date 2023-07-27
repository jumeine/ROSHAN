import time
import sys
sys.path.insert(0, '../../cmake-build-debug')
import drone_state_module as dsm

class Agent():
    def __init__(self):
        print("Agent initialized")

    def update(self, observations, actions, rewards, next_observations, dones):
        pass

    def act(self, action):
        pass

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
