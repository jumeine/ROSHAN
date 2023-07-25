//
// Created by nex on 25.07.23.
//

#ifndef ROSHAN_AGENT_H
#define ROSHAN_AGENT_H

#include <string>
#include <vector>
#include <memory>
#include <deque>
#include "action.h"
#include "state.h"

class Agent {
public:
    virtual ~Agent() = default;
    virtual std::vector<std::shared_ptr<Action>> SelectActions(std::vector<std::deque<std::shared_ptr<State>>> states) = 0;
};

#endif //ROSHAN_AGENT_H
