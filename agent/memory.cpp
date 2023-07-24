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

void Memory::InsertState(std::vector<std::deque<std::shared_ptr<State>>> observation) {
    std::vector<std::pair<double, double>> velocity;
    std::vector<std::pair<double, double>> orientation;
    std::vector<std::vector<std::vector<int>>> terrain;
    std::vector<std::vector<std::vector<int>>> fire_status;
    for(auto &drone : observation) {
        for(auto &state : drone) {
            DroneState* drone_state = dynamic_cast<DroneState*>(state.get());
            velocity.push_back(drone_state->GetVelocity());
            orientation.push_back(drone_state->GetOrientation());
            terrain.push_back(drone_state->GetTerrain());
            fire_status.push_back(drone_state->GetFireStatus());
        }
    }
    velocity_.push_back(velocity);
    orientation_.push_back(orientation);
    terrain_.push_back(terrain);
    fire_status_.push_back(fire_status);
}

void Memory::ClearMemory() {
    velocity_.clear();
    orientation_.clear();
    terrain_.clear();
    fire_status_.clear();
}
