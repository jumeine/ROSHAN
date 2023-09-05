//
// Created by nex on 15.07.23.
//

#include <iostream>
#include <utility>
#include "drone_state.h"

DroneState::DroneState(double speed_x, double speed_y, std::pair<double, double> max_speed, std::vector<std::vector<int>> terrain, std::vector<std::vector<int>> fire_status, std::vector<std::vector<int>> map, std::pair<int, int> position) {
    velocity_.first = speed_x;
    velocity_.second = speed_y;
    terrain_ = std::move(terrain);
    fire_status_ = std::move(fire_status);
    max_speed_ = max_speed;
    map_ = std::move(map);
    position_ = position;
}

DroneState DroneState::GetNewState(double speed_x, double speed_y, std::vector<std::vector<int>> terrain, std::vector<std::vector<int>> fire_status, std::vector<std::vector<int>> updated_map, std::pair<int, int> position) {
    double new_speed_x = velocity_.first + speed_x;
    double new_speed_y = velocity_.second + speed_y;
    new_speed_x = (new_speed_x < max_speed_.first) ? new_speed_x : max_speed_.first;
    new_speed_x = (new_speed_x > -max_speed_.first) ? new_speed_x : -max_speed_.first;
    new_speed_y = (new_speed_y < max_speed_.second) ? new_speed_y : max_speed_.second;
    new_speed_y = (new_speed_y > -max_speed_.second) ? new_speed_y : -max_speed_.second;
    return DroneState(new_speed_x, new_speed_y, max_speed_, std::move(terrain), std::move(fire_status), std::move(updated_map), position);
}

std::pair<double, double> DroneState::GetNewVelocity(double speed_x, double speed_y) {
    double new_speed_x = velocity_.first + speed_x;
    double new_speed_y = velocity_.second + speed_y;
    new_speed_x = (new_speed_x < max_speed_.first) ? new_speed_x : max_speed_.first;
    new_speed_x = (new_speed_x > -max_speed_.first) ? new_speed_x : -max_speed_.first;
    new_speed_y = (new_speed_y < max_speed_.second) ? new_speed_y : max_speed_.second;
    new_speed_y = (new_speed_y > -max_speed_.second) ? new_speed_y : -max_speed_.second;

    return std::make_pair(new_speed_x, new_speed_y);
}
