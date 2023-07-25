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
