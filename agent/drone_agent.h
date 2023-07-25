//
// Created by nex on 25.07.23.
//

#ifndef ROSHAN_DRONE_AGENT_H
#define ROSHAN_DRONE_AGENT_H

#include "agent.h"
#include "drone.h"
#include "drone_state.h"
#include "drone_action.h"
#include <string>
#include <deque>

class DroneNetworkAgent : public Agent {
public:
    DroneNetworkAgent() = default;
    ~DroneNetworkAgent() = default;
    std::vector<std::shared_ptr<Action>> SelectActions(std::vector<std::deque<std::shared_ptr<State>>> states) override;
};


#endif //ROSHAN_DRONE_AGENT_H
