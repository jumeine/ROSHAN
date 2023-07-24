//
// Created by nex on 20.07.23.
//

#ifndef ROSHAN_MEMORY_H
#define ROSHAN_MEMORY_H

#include <vector>
#include <deque>
#include "drone_state.h"
#include <memory>


class Memory {
public:
    Memory() = default;
    explicit Memory(int horizon);

    void InsertState(std::vector<std::deque<std::shared_ptr<State>>> observation);
    void ClearMemory();
    ~Memory() = default;

private:
    std::vector<std::vector<std::pair<double, double>>> velocity_;
    std::vector<std::vector<std::pair<double, double>>> orientation_;
    std::vector<std::vector<std::vector<std::vector<int>>>> terrain_;
    std::vector<std::vector<std::vector<std::vector<int>>>> fire_status_;
};


#endif //ROSHAN_MEMORY_H
