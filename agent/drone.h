//
// Created by nex on 13.07.23.
//

#ifndef ROSHAN_DRONE_H
#define ROSHAN_DRONE_H

#include <utility>
#include <SDL.h>
#include <deque>
#include "rendering/DroneRenderer.h"
#include "drone_state.h"

class DroneAgent {
public:
    DroneAgent() = default;
    explicit DroneAgent(SDL_Renderer* renderer);
    ~DroneAgent() = default;
    std::deque<DroneState> GetStates() { return drone_states_; }
    void Update(double angular, double linear, std::vector<std::vector<int>> terrain, std::vector<std::vector<int>> fire_status);
    void Move(double angular, double linear);
    std::pair<double, double> GetPosition() { return position_; }
    int GetViewRange() { return view_range_; }
    void Render(std::pair<int, int> position, int size);
    void Initialize(std::vector<std::vector<int>> terrain, std::vector<std::vector<int>> fire_status);
private:
    void UpdateStates(double angular, double linear, std::vector<std::vector<int>> terrain, std::vector<std::vector<int>> fire_status);
    std::deque<DroneState> drone_states_;
    std::pair<double, double> position_; // x, y
    int view_range_;
    std::pair<double, double> velocity_; // angular & linear
    DroneRenderer renderer_;
};


#endif //ROSHAN_DRONE_H