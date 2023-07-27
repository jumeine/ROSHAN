//
// Created by nex on 25.07.23.
//

#include "drone_agent.h"

std::vector<std::shared_ptr<Action>> DroneNetworkAgent::SelectActions(std::vector<std::deque<std::shared_ptr<State>>> states) {
    std::vector<std::shared_ptr<Action>> actions;
    for (auto &drone : states) {
//        std::pair<double, double> velocity = std::dynamic_pointer_cast<DroneState>(drone[0])->GetVelocity();
        std::pair<double, double> velocity = std::make_pair(0.0, 0.0);
        actions.push_back(std::make_shared<DroneAction>(velocity.first, velocity.second));
    }
    return actions;
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
