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

void DroneAgent::Update(double x, double y) {
    position_.first += x;
    position_.second += y;
}
