//
// Created by nex on 20.07.23.
//

#include "memory.h"

Memory::Memory(int horizon) {
    // Reserve memory for the horizon
    velocity_.reserve(horizon);
    orientation_.reserve(horizon);
    terrain_.reserve(horizon);
    fire_status_.reserve(horizon);
}

void Memory::InsertState(std::vector<std::deque<std::shared_ptr<State>>> observation,
                         std::vector<std::shared_ptr<Action>> action,
                         std::vector<double> reward,
                         std::vector<std::deque<std::shared_ptr<State>>> next_observation,
                         std::vector<bool> done) {
    // Insert the state into the memory
    std::vector<std::pair<double, double>> velocity;
    std::vector<std::pair<double, double>> orientation;
    std::vector<std::vector<std::vector<int>>> terrain;
    std::vector<std::vector<std::vector<int>>> fire_status;

    std::vector<std::pair<double, double>> velocity_next;
    std::vector<std::pair<double, double>> orientation_next;
    std::vector<std::vector<std::vector<int>>> terrain_next;
    std::vector<std::vector<std::vector<int>>> fire_status_next;

    // Implement when needed
}

void Memory::ClearMemory() {
    velocity_.clear();
    orientation_.clear();
    terrain_.clear();
    fire_status_.clear();
}
