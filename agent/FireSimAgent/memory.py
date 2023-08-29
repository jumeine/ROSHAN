# A memory for the drone states

class Memory:
    def __init__(self, horizon):
        self.horizon = horizon
        self.actions = []
        self.states = []
        self.next_states = []
        self.logprobs = []
        self.rewards = []
        self.is_terminals = []

    def is_ready_to_train(self):
        return len(self.actions) >= self.horizon

    def add(self, states, actions, logprobs, rewards, next_states, terminals):
        """
        Adds the given state, action, log probability, reward, and terminal flag to the memory.

        :param states: The state to add.
        :param actions: The action to add.
        :param logprobs: The log probability to add.
        :param rewards: The reward to add.
        :param next_states: The next state to add.
        :param terminals: The terminal flag to add.
        :return: None.
        """
        for state, action, logprob, reward, next_state, terminal in zip(states, actions, logprobs, rewards, next_states, terminals):
            self.states.append(state)
            self.actions.append(action)
            self.logprobs.append(logprob)
            self.rewards.append(reward)
            self.next_states.append(next_state)
            self.is_terminals.append(terminal)

    def clear_memory(self):
        del self.actions[:]
        del self.states[:]
        del self.next_states[:]
        del self.logprobs[:]
        del self.rewards[:]
        del self.is_terminals[:]
