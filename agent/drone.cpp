//
// Created by nex on 13.07.23.
//

#include "drone.h"

DroneAgent::DroneAgent(std::shared_ptr<SDL_Renderer> renderer) {
    renderer_ = DroneRenderer(renderer);
    position_ = std::make_pair(30.0, 30.0);
    view_range_ = 20;
}

void DroneAgent::Render(std::pair<int, int> position, int size) {
    double angle = drone_states_[0].GetVelocity().first;
    renderer_.Render(position, size, view_range_, angle);
}


void DroneAgent::Move(double angular, double linear) {
    std::pair<double, double> orientation_vector = drone_states_[0].GetNewOrientation(angular);
    std::pair<double, double> velocity_vector = drone_states_[0].GetNewVelocity(angular, linear);
    position_.first += velocity_vector.second * orientation_vector.first;
    position_.second += velocity_vector.second * orientation_vector.second;
}

void DroneAgent::Update(double angular, double linear, std::vector<std::vector<int>> terrain, std::vector<std::vector<int>> fire_status) {
    UpdateStates(angular, linear, terrain, fire_status);
}

void DroneAgent::Initialize(std::vector<std::vector<int>> terrain, std::vector<std::vector<int>> fire_status) {
    for(int i = 0; i < 4; ++i) {
        DroneState new_state = drone_states_[0].GetNewState(0, 0, terrain, fire_status);
        drone_states_.push_front(new_state);
    }
}

void DroneAgent::UpdateStates(double angular, double linear, std::vector<std::vector<int>> terrain, std::vector<std::vector<int>> fire_status) {
    // Update the states, last state get's kicked out
    DroneState new_state = drone_states_[0].GetNewState(angular, linear, terrain, fire_status);
    drone_states_.push_front(new_state);

    // Maximum number of states i.e. memory
    if (drone_states_.size() > 4) {
        drone_states_.pop_back();
    }
}
