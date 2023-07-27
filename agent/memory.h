//
// Created by nex on 20.07.23.
//

#ifndef ROSHAN_MEMORY_H
#define ROSHAN_MEMORY_H

#include <vector>
#include <deque>
#include "drone_state.h"
#include "drone_action.h"
#include <memory>


class Memory {
public:
    Memory() = default;
    explicit Memory(int horizon);

    void InsertState(std::vector<std::deque<std::shared_ptr<State>>> observation,
                     std::vector<std::shared_ptr<Action>> action,
                     std::vector<double> reward,
                     std::vector<std::deque<std::shared_ptr<State>>> next_observation,
                     std::vector<bool> done);
    bool IsReadyToTrain();
    std::tuple<
            std::vector<std::vector<std::shared_ptr<State>>>,
            std::vector<std::shared_ptr<Action>>,
            std::vector<double>,
            std::vector<std::vector<std::shared_ptr<State>>>,
            std::vector<bool>
    > SampleBatch();
    void ClearMemory();
    ~Memory() = default;

private:
    int horizon_;

    std::vector<std::deque<std::shared_ptr<State>>> observations_;
    std::vector<std::shared_ptr<Action>> actions_;
    std::vector<double> rewards_;
    std::vector<std::deque<std::shared_ptr<State>>> next_observations_;
    std::vector<bool> dones_;
};


#endif //ROSHAN_MEMORY_H
