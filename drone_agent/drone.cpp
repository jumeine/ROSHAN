//
// Created by nex on 13.07.23.
//

#include <iostream>
#include "drone.h"

DroneAgent::DroneAgent(std::shared_ptr<SDL_Renderer> renderer, FireModelParameters &parameters, int id) : id_(id), parameters_(parameters) {
    renderer_ = DroneRenderer(renderer);
    int x = 20; // position in grid
    int y = 20; // position in grid
    position_ = std::make_pair(x * parameters_.GetCellSize(), y * parameters_.GetCellSize());
    view_range_ = 20;
}

void DroneAgent::Render(std::pair<int, int> position, int size) {
    renderer_.Render(position, size, view_range_, 0);
}


void DroneAgent::Move(double speed_x, double speed_y) {
    std::pair<double, double> velocity_vector = drone_states_[0].GetNewVelocity(speed_x, speed_y);
    position_.first += velocity_vector.first * parameters_.GetDt();
    position_.second += velocity_vector.second * parameters_.GetDt();
}

void DroneAgent::Update(double speed_x, double speed_y, std::vector<std::vector<int>> terrain, std::vector<std::vector<int>> fire_status) {
    UpdateStates(speed_x, speed_y, terrain, fire_status);
}

void DroneAgent::Initialize(std::vector<std::vector<int>> terrain, std::vector<std::vector<int>> fire_status) {
    for(int i = 0; i < 4; ++i) {
        DroneState new_state = DroneState(0, 0, parameters_.GetMaxVelocity(), terrain, fire_status);
        drone_states_.push_front(new_state);
    }
}

void DroneAgent::UpdateStates(double speed_x, double speed_y, std::vector<std::vector<int>> terrain, std::vector<std::vector<int>> fire_status) {
    // Update the states, last state get's kicked out
    DroneState new_state = drone_states_[0].GetNewState(speed_x, speed_y, terrain, fire_status);
    drone_states_.push_front(new_state);

    // Maximum number of states i.e. memory
    if (drone_states_.size() > 4) {
        drone_states_.pop_back();
    }
}

bool DroneAgent::DispenseWater(int water_dispense, GridMap &grid_map) {
    if (water_dispense == 1) {
        std::pair<int, int> grid_position = GetGridPosition();
        grid_map.WaterDispension(grid_position.first, grid_position.second);
        return true;
    }
    return false;
}

std::pair<double, double> DroneAgent::GetGridPositionDouble() {
    double x, y;
    x = position_.first / parameters_.GetCellSize();
    y = position_.second / parameters_.GetCellSize();
    return std::make_pair(x, y);
}

std::pair<double, double> DroneAgent::GetRealPosition() {
    return position_;
}

std::pair<int, int> DroneAgent::GetGridPosition() {
    int x, y;
    parameters_.ConvertRealToGridCoordinates(position_.first, position_.second, x, y);
    return std::make_pair(x, y);
}

//DroneAgent::~DroneAgent() {
//    drone_states_.clear();
//
//}
