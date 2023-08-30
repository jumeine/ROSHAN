//
// Created by nex on 15.07.23.
//

#include <iostream>
#include "drone_state.h"

DroneState::DroneState() {
    velocity_.first = 0;
    velocity_.second = 0;

    max_speed_.first = 0;
    max_speed_.second = 0;

    // Initialize terrain with 0
    for (auto &i : terrain_) {
        for (auto &j : i) {
            j = 0;
        }
    }
    // Initialize fire status with 0
    for (auto &i : fire_status_) {
        for (auto &j : i) {
            j = 0;
        }
    }

}

DroneState::DroneState(double speed_x, double speed_y, std::pair<double, double> max_speed, std::vector<std::vector<int>> terrain, std::vector<std::vector<int>> fire_status) {
    velocity_.first = speed_x;
    velocity_.second = speed_y;
    terrain_ = terrain;
    fire_status_ = fire_status;
    max_speed_ = max_speed;
}

DroneState DroneState::GetNewState(double speed_x, double speed_y, std::vector<std::vector<int>> terrain, std::vector<std::vector<int>> fire_status) {
    double new_speed_x = velocity_.first + speed_x;
    double new_speed_y = velocity_.second + speed_y;
    new_speed_x = (new_speed_x < max_speed_.first) ? new_speed_x : max_speed_.first;
    new_speed_x = (new_speed_x > -max_speed_.first) ? new_speed_x : -max_speed_.first;
    new_speed_y = (new_speed_y < max_speed_.second) ? new_speed_y : max_speed_.second;
    new_speed_y = (new_speed_y > -max_speed_.second) ? new_speed_y : -max_speed_.second;
    return DroneState(new_speed_x, new_speed_y, max_speed_, terrain, fire_status);
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
