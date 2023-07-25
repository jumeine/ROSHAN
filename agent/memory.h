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
    void ClearMemory();
    ~Memory() = default;

private:
    std::vector<std::vector<std::pair<double, double>>> velocity_;
    std::vector<std::vector<std::pair<double, double>>> orientation_;
    std::vector<std::vector<std::vector<std::vector<int>>>> terrain_;
    std::vector<std::vector<std::vector<std::vector<int>>>> fire_status_;

    std::vector<std::vector<std::pair<double, double>>> velocity_next_;
    std::vector<std::vector<std::pair<double, double>>> orientation_next_;
    std::vector<std::vector<std::vector<std::vector<int>>>> terrain_next_;
    std::vector<std::vector<std::vector<std::vector<int>>>> fire_status_next_;

    std::vector<std::vector<DroneAction>> action_;
    std::vector<std::vector<double>> reward_;
    std::vector<std::vector<bool>> done_;
};


#endif //ROSHAN_MEMORY_H
