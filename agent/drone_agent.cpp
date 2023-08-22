//
// Created by nex on 25.07.23.
//

#include "drone_agent.h"

std::vector<std::shared_ptr<Action>> DroneNetworkAgent::SelectActions(std::vector<std::deque<std::shared_ptr<State>>> observations) {
    std::vector<std::shared_ptr<Action>> actions;
    for (auto &drone : observations) {
//        std::pair<double, double> velocity = std::dynamic_pointer_cast<DroneState>(drone[0])->GetVelocity();
        std::pair<double, double> velocity = std::make_pair(0.0, 0.0);
        actions.push_back(std::make_shared<DroneAction>(velocity.first, velocity.second));
    }
    auto data = RestructureData(observations);
    auto result = (*agent_).attr("act")(data).cast<std::pair<py::array_t<float>, py::array_t<float>>>();
    double action = static_cast<double>(result.first.at(0));
    double action2 = static_cast<double>(result.second.at(0));
    return actions;
}

std::tuple<
        std::vector<std::vector<std::vector<std::vector<int>>>>,
        std::vector<std::vector<std::vector<std::vector<int>>>>,
        std::vector<std::vector<std::pair<double, double>>>,
        std::vector<std::vector<std::pair<double, double>>>
>  DroneNetworkAgent::RestructureData(std::vector<std::deque<std::shared_ptr<State>>> observations) {
    std::vector<std::vector<DroneState>> simple_structure;

    for(const auto& deque : observations) {
        std::vector<DroneState> vec;
        for (const auto& ptr : deque) {
            vec.push_back(*std::dynamic_pointer_cast<DroneState>(ptr));
        }
        simple_structure.push_back(vec);
    }

    std::vector<std::vector<std::vector<std::vector<int>>>> all_terrains;
    std::vector<std::vector<std::vector<std::vector<int>>>> all_fire_statuses;
    std::vector<std::vector<std::pair<double, double>>> all_velocities;
    std::vector<std::vector<std::pair<double, double>>> all_orientations;

    for (const auto& drone_states : simple_structure) {
        std::vector<std::vector<std::vector<int>>>  terrains;
        std::vector<std::vector<std::vector<int>>>  fire_statuses;
        std::vector<std::pair<double, double>> velocities;
        std::vector<std::pair<double, double>> orientations;
        for (DroneState state : drone_states) {
            terrains.push_back(state.GetTerrain());
            fire_statuses.push_back(state.GetFireStatus());
            velocities.push_back(state.GetVelocity());
            orientations.push_back(state.GetOrientation());
        }
        all_terrains.push_back(terrains);
        all_fire_statuses.push_back(fire_statuses);
        all_velocities.push_back(velocities);
        all_orientations.push_back(orientations);
    }

    std::tuple<
            std::vector<std::vector<std::vector<std::vector<int>>>>,
            std::vector<std::vector<std::vector<std::vector<int>>>>,
            std::vector<std::vector<std::pair<double, double>>>,
            std::vector<std::vector<std::pair<double, double>>>
    > data(all_terrains, all_fire_statuses, all_velocities, all_orientations);

    return data;
}

void DroneNetworkAgent::Update(std::vector<std::vector<std::shared_ptr<State>>> observations,
                               std::vector<std::shared_ptr<Action>> actions,
                               std::vector<double> rewards,
                               std::vector<std::vector<std::shared_ptr<State>>> next_observations,
                               std::vector<bool> dones) {
    // Convert ever State in observations to a DroneState
    std::vector<std::vector<DroneState>> drone_obs;
    for (auto &drone : observations) {
        std::vector<DroneState> drone_state;
        for (auto &state : drone) {
            drone_state.push_back(*std::dynamic_pointer_cast<DroneState>(state));
        }
        drone_obs.push_back(drone_state);
    }
    // Convert every Action in actions to a DroneAction
    std::vector<DroneAction> drone_actions;
    for (auto &action : actions) {
        drone_actions.push_back(*std::dynamic_pointer_cast<DroneAction>(action));
    }

    // Convert every State in next_observations to a DroneState
    std::vector<std::vector<DroneState>> drone_next_obs;
    for (auto &drone : next_observations) {
        std::vector<DroneState> drone_state;
        for (auto &state : drone) {
            drone_state.push_back(*std::dynamic_pointer_cast<DroneState>(state));
        }
        drone_next_obs.push_back(drone_state);
    }
    (*agent_).attr("update")(drone_obs, drone_actions, rewards, drone_next_obs, dones);
}

DroneNetworkAgent::DroneNetworkAgent() : guard(){  // Initialize guard in the constructor initializer list
    py::module::import("sys").attr("path").attr("insert")(0, "../agent/FireSimAgent");
    py::object Agent = py::module::import("agent").attr("Agent");
    agent_ = std::make_shared<py::object>(Agent());
}
