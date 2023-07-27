//
// Created by nex on 20.07.23.
//

#include "memory.h"

Memory::Memory(int horizon) {
    horizon_ = horizon;
    // Reserve memory for the horizon
    observations_.reserve(horizon);
    actions_.reserve(horizon);
    rewards_.reserve(horizon);
    next_observations_.reserve(horizon);
    dones_.reserve(horizon);
}

void Memory::InsertState(std::vector<std::deque<std::shared_ptr<State>>> observation,
                         std::vector<std::shared_ptr<Action>> action,
                         std::vector<double> reward,
                         std::vector<std::deque<std::shared_ptr<State>>> next_observation,
                         std::vector<bool> done) {
    // Get the size of the batch
    int batch_size = observation.size();
    for(int i = 0; i < batch_size; i++) {
        // Insert the states
        observations_.push_back(observation[i]);
        actions_.push_back(action[i]);
        rewards_.push_back(reward[i]);
        next_observations_.push_back(next_observation[i]);
        dones_.push_back(done[i]);
    }

}

void Memory::ClearMemory() {
    observations_.clear();
    actions_.clear();
    rewards_.clear();
    next_observations_.clear();
    dones_.clear();
}

// Returns whether the memory is ready to train
bool Memory::IsReadyToTrain() {
    return observations_.size() >= horizon_;
}

std::tuple<
        std::vector<std::vector<std::shared_ptr<State>>>,
        std::vector<std::shared_ptr<Action>>,
        std::vector<double>,
        std::vector<std::vector<std::shared_ptr<State>>>,
        std::vector<bool>
> Memory::SampleBatch()
{
    std::vector<std::vector<std::shared_ptr<State>>> flat_observations;
    std::vector<std::vector<std::shared_ptr<State>>> flat_next_observations;

    // Unroll each deque into a vector
    for (const auto& deque : observations_) {
        std::vector<std::shared_ptr<State>> vec;
        for (const auto& ptr : deque) {
            vec.push_back(ptr);
        }
        flat_observations.push_back(vec);
    }

    // Unroll each deque into a vector
    for (const auto& deque : next_observations_) {
        std::vector<std::shared_ptr<State>> vec;
        for (const auto& ptr : deque) {
            vec.push_back(ptr);
        }
        flat_next_observations.push_back(vec);
    }

    return std::make_tuple(flat_observations, actions_, rewards_, flat_next_observations, dones_);
}
