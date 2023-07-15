//
// Created by nex on 15.07.23.
//

#ifndef ROSHAN_DRONE_STATE_H
#define ROSHAN_DRONE_STATE_H

#include <utility>
#include <vector>
#include <cmath>
#include <array>

class DroneState {
public:
    DroneState();
    DroneState(double angular, double linear);
    ~DroneState() = default;
    DroneState GetNewState(double angular, double linear);
    void SetOrientation() { orientation_vector_.first = cos(velocity_.first); orientation_vector_.second = sin(velocity_.first); }
    std::pair<double, double> GetOrientation() { return orientation_vector_; }
    void SetVelocity(double angular, double linear) { velocity_.first = angular; velocity_.second = linear; }
    std::pair<double, double> GetVelocity() { return velocity_; }
private:
    std::pair<double, double> velocity_; // angular & linear
    std::array<std::array<int, 20>, 20> terrain_;
    std::array<std::array<int, 20>, 20> fire_status_;
    std::pair<double, double> orientation_vector_; // x, y
};


#endif //ROSHAN_DRONE_STATE_H
