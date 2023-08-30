import sys
import os
import numpy as np

# Add path to module directories #TODO make this more elegant
module_directory = '../../cmake-build-debug'
sys.path.insert(0, module_directory)

import firesim
from agent import Agent
from memory import Memory

# Change the current working directory, so that the python script has the same folder as the c++ executable
os.chdir(module_directory)


def restructure_data(observations):
    # Initialize lists to store each drone's attributes.
    all_terrains, all_fire_statuses, all_velocities, all_orientations = [], [], [], []

    for deque in observations:
        # Filter out instances that aren't of type firesim.DroneState.
        drone_states = np.array([state for state in deque if isinstance(state, firesim.DroneState)])

        if len(drone_states) == 0:
            continue

        # Use numpy vectorization to get the attributes.
        terrains = np.array([state.GetTerrain() for state in drone_states])
        fire_statuses = np.array([state.GetFireStatus() for state in drone_states])
        velocities = np.array([state.GetVelocity() for state in drone_states])
        # orientations = np.array([state.GetOrientation() for state in drone_states])

        all_terrains.append(terrains)
        all_fire_statuses.append(fire_statuses)
        all_velocities.append(velocities)
        # all_orientations.append(orientations)

    # Convert lists to numpy arrays for the final output.
    all_terrains = np.array(all_terrains)
    all_fire_statuses = np.array(all_fire_statuses)
    all_velocities = np.array(all_velocities)
    # all_orientations = np.array(all_orientations)

    data = (all_terrains, all_fire_statuses, all_velocities)
    return data


# Press the green button in the gutter to run the script.
if __name__ == '__main__':

    # Lists alls the functions in the EngineCore class
    # print(dir(EngineCore))

    engine = firesim.EngineCore()
    agent = Agent()
    memory = Memory(5000)
    engine.Init(0)

    while engine.IsRunning():
        engine.HandleEvents()
        engine.Update()
        engine.Render()
        if engine.AgentIsRunning():
            observations = engine.GetObservations()
            obs = restructure_data(observations)
            actions, action_logprobs = agent.act(obs)
            drone_actions = []
            for activation in actions:
                drone_actions.append(firesim.DroneAction(activation[0], activation[1], activation[2]))
            next_observations, rewards, terminals = engine.Step(drone_actions)
            memory.add(observations, actions, action_logprobs, rewards, next_observations, terminals)
            if memory.is_ready_to_train():
                # agent.update(memory)
                memory.clear_memory()
            # TODO Update Agent

    engine.Clean()
