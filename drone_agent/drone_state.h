//
// Created by nex on 15.07.23.
//

#ifndef ROSHAN_DRONE_STATE_H
#define ROSHAN_DRONE_STATE_H

#include <utility>
#include <vector>
#include <cmath>
#include <array>
#include "../state.h"


class DroneState : public State{
public:
    explicit DroneState(double speed_x, double speed_y, std::pair<double, double> max_speed, std::vector<std::vector<int>> terrain, std::vector<std::vector<int>> fire_status, std::vector<std::vector<int>> map, std::pair<int, int> position);
    DroneState GetNewState(double speed_x, double speed_y, std::vector<std::vector<int>> terrain, std::vector<std::vector<int>> fire_status, std::vector<std::vector<int>> updated_map, std::pair<int, int> position);
//    void SetOrientation() { orientation_vector_.first = cos(velocity_.first); orientation_vector_.second = sin(velocity_.first); }
//    std::pair<double, double> GetOrientation() { return orientation_vector_; }
//    std::pair<double, double> GetNewOrientation(double angular) { return std::make_pair(cos(velocity_.first + angular), sin(velocity_.first + angular)); }
    void SetVelocity(double speed_x, double speed_y) { velocity_.first = speed_x; velocity_.second = speed_y; }
    std::pair<double, double> GetVelocity() { return velocity_; }
    std::pair<double, double> GetNewVelocity(double speed_x, double speed_y);
    std::vector<std::vector<int>> GetTerrain() { return terrain_; }
    std::vector<std::vector<int>> GetFireStatus() { return fire_status_; }
    std::vector<std::vector<int>> GetMap() { return map_; }
    std::pair<int, int> GetPosition() { return position_; }
    // For python visibility
    std::pair<double, double> get_velocity() const { return velocity_; }
    std::vector<std::vector<int>> get_terrain() const { return terrain_; }
    std::vector<std::vector<int>> get_fire_status() const { return fire_status_; }
    std::vector<std::vector<int>> get_map() const { return map_; }
    std::pair<int, int> get_position() const { return position_; }
    std::pair<double, double> get_orientation_vector() const { return orientation_vector_; }
private:
    std::pair<double, double> velocity_; // speed_x, speed_y
    std::pair<double, double> max_speed_;
    std::pair<int, int> position_; // x, y
    std::vector<std::vector<int>> terrain_;
    std::vector<std::vector<int>> fire_status_;
    std::vector<std::vector<int>> map_;
    std::pair<double, double> orientation_vector_; // x, y
};

#endif //ROSHAN_DRONE_STATE_H
