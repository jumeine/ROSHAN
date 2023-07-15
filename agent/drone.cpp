//
// Created by nex on 13.07.23.
//

#include "drone.h"

DroneAgent::DroneAgent(SDL_Renderer *renderer) {
    renderer_ = DroneRenderer(renderer);
    position_ = std::make_pair(30.0, 30.0);
}

void DroneAgent::Render(std::pair<int, int> position, int size) {
    renderer_.Render(position, size);
}

void DroneAgent::Update(double angular, double linear) {
    UpdateStates(angular, linear);
    std::pair<double, double> orientation_vector = drone_states_[0].GetOrientation();
    std::pair<double, double> velocity_vector = drone_states_[0].GetVelocity();
    position_.first += velocity_vector.second * orientation_vector.first;
    position_.second += velocity_vector.second * orientation_vector.second;
}

void DroneAgent::Initialize() {

}

void DroneAgent::UpdateStates(double angular, double linear) {
    // Update the states, last state get's kicked out
    DroneState new_state = drone_states_[0].GetNewState(angular, linear);
    drone_states_.push_front(new_state);

    // Maximum number of states i.e. memory
    if (drone_states_.size() > 4) {
        drone_states_.pop_back();
    }
}
