import time

class Agent():
    def __init__(self):
        pass

    def act(self, state):
        print("Agent acting on state: ", state)

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
