//
// Created by nex on 15.07.23.
//

#include "drone_state.h"

DroneState::DroneState() {
    SetVelocity(0, 0);
    SetOrientation();

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

DroneState::DroneState(double angular, double linear) {
    SetVelocity(angular, linear);
    SetOrientation();

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

DroneState DroneState::GetNewState(double angular, double linear) {
    double new_angular = velocity_.first + angular;
    double new_linear = velocity_.second + linear;
    return DroneState(new_angular, new_linear);
}
