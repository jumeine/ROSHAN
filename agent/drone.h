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
    void Update(double angular, double linear);
    std::pair<double, double> GetPosition() { return position_; }
    void Render(std::pair<int, int> position, int size);
    void Initialize();
private:
    void UpdateStates(double angular, double linear);
    std::deque<DroneState> drone_states_;
    std::pair<double, double> position_; // x, y
    std::pair<double, double> velocity_; // angular & linear
    DroneRenderer renderer_;
};


#endif //ROSHAN_DRONE_H
