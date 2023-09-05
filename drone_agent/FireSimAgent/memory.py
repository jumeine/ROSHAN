# A memory for the drone states

class Memory:
    def __init__(self, horizon):
        self.horizon = horizon
        self.actions = []

        # States
        self.terrain = []
        self.fire_status = []
        self.velocity = []
        self.map = []
        self.position = []

        # Next states
        self.next_terrain = []
        self.next_fire_status = []
        self.next_velocity = []
        self.next_map = []
        self.next_position = []

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
        terrains, fire_statuses, velocities, maps, positions = states
        next_terrains, next_fire_statuses, next_velocities, next_maps, next_positions = next_states
        for terrain, fire_status, velocity, map, position, action, logprob, reward, next_terrain, next_fire_status, \
            next_velocity, next_map, next_position, terminal in zip(terrains, fire_statuses, velocities, maps,
                                                                    positions,
                                                                    actions, logprobs, rewards, next_terrains,
                                                                    next_fire_statuses,
                                                                    next_velocities, next_maps, next_positions,
                                                                    terminals):
            self.terrain.append(terrain)
            self.fire_status.append(fire_status)
            self.velocity.append(velocity)
            self.map.append(map)
            self.position.append(position)

            self.actions.append(action)
            self.logprobs.append(logprob)
            self.rewards.append(reward)
            self.is_terminals.append(terminal)

            self.next_terrain.append(next_terrain)
            self.next_fire_status.append(next_fire_status)
            self.next_velocity.append(next_velocity)
            self.next_map.append(next_map)
            self.next_position.append(next_position)

    def clear_memory(self):
        del self.actions[:]
        del self.logprobs[:]
        del self.rewards[:]
        del self.is_terminals[:]

        del self.terrain[:]
        del self.fire_status[:]
        del self.velocity[:]
        del self.map[:]
        del self.position[:]

        del self.next_terrain[:]
        del self.next_fire_status[:]
        del self.next_velocity[:]
        del self.next_map[:]
        del self.next_position[:]

