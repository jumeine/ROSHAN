//
// Created by nex on 25.07.23.
//

#ifndef ROSHAN_DRONE_AGENT_H
#define ROSHAN_DRONE_AGENT_H

#include "agent.h"
#include "drone.h"
#include "drone_state.h"
#include "drone_action.h"
#include "externals/pybind11/include/pybind11/embed.h"
#include "externals/pybind11/include/pybind11/stl.h"
#include <string>
#include <deque>

namespace py = pybind11;

class DroneNetworkAgent : public Agent {
public:
    DroneNetworkAgent();
    ~DroneNetworkAgent() = default;
    std::vector<std::shared_ptr<Action>> SelectActions(std::vector<std::deque<std::shared_ptr<State>>> states) override;
    void Update(std::vector<std::vector<std::shared_ptr<State>>> observations,
                std::vector<std::shared_ptr<Action>> actions,
                std::vector<double> rewards,
                std::vector<std::vector<std::shared_ptr<State>>> next_observations,
                std::vector<bool> dones) override;
private:
    std::shared_ptr<py::object> agent_;
    py::scoped_interpreter guard;  // Keep the interpreter alive
};


#endif //ROSHAN_DRONE_AGENT_H
