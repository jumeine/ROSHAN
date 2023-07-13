//
// Created by nex on 13.07.23.
//

#ifndef ROSHAN_DRONE_H
#define ROSHAN_DRONE_H

#include <utility>
#include <SDL.h>
#include "rendering/DroneRenderer.h"

class DroneAgent {
public:
    DroneAgent() = default;
    DroneAgent(SDL_Renderer* renderer);
    ~DroneAgent() = default;
    void Update(double x, double y);
    std::pair<double, double> GetPosition() { return position_; }
    void Render(std::pair<int, int> position, int size);
    void Initialize();
private:
    std::pair<double, double> position_;
    DroneRenderer renderer_;
};


#endif //ROSHAN_DRONE_H
