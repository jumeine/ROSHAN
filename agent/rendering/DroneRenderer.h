//
// Created by nex on 13.07.23.
//

#ifndef ROSHAN_DRONERENDERER_H
#define ROSHAN_DRONERENDERER_H

#include <SDL.h>

class DroneRenderer {
public:
    DroneRenderer() = default;
    DroneRenderer(SDL_Renderer* renderer);
    ~DroneRenderer() = default;
    void Render(std::pair<int, int> position, int size, int view_range, double angle);
    void init();
private:
    SDL_Texture* drone_texture_;
    SDL_Renderer* renderer_;
};


#endif //ROSHAN_DRONERENDERER_H
