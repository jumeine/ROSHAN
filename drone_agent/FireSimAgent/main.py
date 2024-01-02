import sys
import os
import warnings
import torch
import numpy as np

# Add path to module directories #TODO make this more elegant
script_directory = os.path.dirname(os.path.abspath(__file__))
module_directory = os.path.join(script_directory, '../../build/')
sys.path.insert(0, module_directory)

import firesim
from agent import Agent
from memory import Memory
from utils import Logger
import cProfile

# Change the current working directory, so that the python script has the same folder as the c++ executable
os.chdir(module_directory)


def restructure_data(observations_):
    all_terrains, all_fire_statuses, all_velocities, all_maps, all_positions = [], [], [], [], []

    for deque in observations_:
        drone_states = np.array([state for state in deque if isinstance(state, firesim.DroneState)])
        if len(drone_states) == 0:
            continue

        terrains = np.array([state.GetTerrainNorm() for state in drone_states])
        fire_statuses = np.array([state.GetFireStatus() for state in drone_states])
        velocities = np.array([state.GetVelocityNorm() for state in drone_states])
        maps = np.array([state.GetMap() for state in drone_states])
        positions = np.array([state.GetPositionNorm() for state in drone_states])

        all_terrains.append(terrains)
        all_fire_statuses.append(fire_statuses)
        all_velocities.append(velocities)
        all_maps.append(maps)
        all_positions.append(positions)

    return np.array(all_terrains), np.array(all_fire_statuses), np.array(all_velocities), np.array(all_maps), np.array(all_positions)


# Press the green button in the gutter to run the script.
if __name__ == '__main__':

    # Lists alls the functions in the EngineCore class
    # print(dir(EngineCore))
    horizon = 256#8192
    batch_size = 16#64
    t = -1

    engine = firesim.EngineCore()
    memory = Memory()
    logger = Logger(log_dir='./logs', log_interval=1)
    agent = Agent('ppo', logger)
    logger.set_logging(True)
    if memory.max_size <= horizon:
        warnings.warn("Memory size is smaller than horizon. Setting horizon to memory size.")
        horizon = memory.max_size - 1
    engine.Init(0)

    while engine.IsRunning():
        engine.HandleEvents()
        engine.Update()
        engine.Render()
        if engine.AgentIsRunning():
            t += 1 #TODO use simulation time instead of timesteps
            observations = engine.GetObservations()
            obs = restructure_data(observations)
            actions, action_logprobs = agent.act(obs, t)
            drone_actions = []
            for activation in actions:
                drone_actions.append(firesim.DroneAction(activation[0], activation[1], int(activation[2])))
            next_observations, rewards, terminals = engine.Step(drone_actions)
            next_obs = restructure_data(next_observations)
            memory.add(obs, actions, action_logprobs, rewards, next_obs, terminals)
            if agent.should_train(memory, horizon, t):
                agent.update(memory, batch_size)
                logger.log()

    engine.Clean()

    # agent = Agent('ppo')
    # ppo = agent.algorithm
    #
    # ppo.gamma = 1
    # rewards = torch.tensor(np.array([1, 1, 1, 1]), dtype=torch.float32).to('cuda')
    # masks = torch.tensor(np.array([1, 1, 1, 0]), dtype=torch.float32).to('cuda')
    # values = torch.tensor(np.array([0.5, 1.2, 4, 0.4]), dtype=torch.float32).to('cuda')
    # returns = ppo.calculate_returns(rewards)
    # ppo.gamma = 0.99
    # ppo._lambda = 0.7
    # adv = ppo.get_advantages(values, masks, rewards)
    # print(returns)
    # print(adv)
