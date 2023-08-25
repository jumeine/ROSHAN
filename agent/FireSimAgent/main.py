import sys
import os
import numpy as np

# Add path to module directories #TODO make this more elegant
module_directory = '../../cmake-build-debug'
sys.path.insert(0, module_directory)
import firesim
from agent import Agent

# Change the current working directory, so that the python script has the same folder as the c++ executable
os.chdir(module_directory)

def restructure_data(observations):
    simple_structure = []

    for deque in observations:
        vec = [state for state in deque if isinstance(state, firesim.DroneState)]
        simple_structure.append(vec)

    all_terrains = []
    all_fire_statuses = []
    all_velocities = []
    all_orientations = []

    for drone_states in simple_structure:
        terrains = []
        fire_statuses = []
        velocities = []
        orientations = []
        for state in drone_states:
            terrains.append(np.asarray(state.GetTerrain()))
            fire_statuses.append(state.GetFireStatus())
            velocities.append(state.GetVelocity())
            orientations.append(state.GetOrientation())
        all_terrains.append(terrains)
        all_fire_statuses.append(fire_statuses)
        all_velocities.append(velocities)
        all_orientations.append(orientations)

    data = (all_terrains, all_fire_statuses, all_velocities, all_orientations)
    return data


# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    #Lists alls the functions in the EngineCore class
    #print(dir(EngineCore))
    engine = firesim.EngineCore()
    agent = Agent()
    engine.Init()

    while(engine.IsRunning()):
        engine.HandleEvents()
        engine.Update()
        engine.Render()
        if engine.AgentIsRunning():
            obs = engine.GetObservations()
            # TODO MAYBE Transpose Terrain Observation so its in the same direction as the simulation
            observations = restructure_data(obs)
            actions = agent.act(observations)
            action = firesim.DroneAction(actions[0][0][0], actions[0][0][1])
            next_observations, rewards, dones = engine.Step([action])
            # TODO Fill Memory
            # TODO Update Agent


    engine.Clean()
