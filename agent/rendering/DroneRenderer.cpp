//
// Created by nex on 13.07.23.
//

#include "DroneRenderer.h"
#include <SDL_image.h>

DroneRenderer::DroneRenderer(SDL_Renderer *renderer) {
    renderer_ = renderer;

    // Load the arrow texture
    SDL_Surface *drone_surface = IMG_Load("../assets/drone.png");
    if (drone_surface == NULL) {
        SDL_Log("Unable to load image: %s", SDL_GetError());
        return;
    }
    drone_texture_ = SDL_CreateTextureFromSurface(renderer_, drone_surface);
    SDL_FreeSurface(drone_surface);
}

void DroneRenderer::Render(std::pair<int, int> position, int size) {
    // Render the arrow
    SDL_Rect destRect = {position.first, position.second, size, size}; // x, y, width and height of the arrow
    SDL_RenderCopyEx(renderer_, drone_texture_, NULL, &destRect, 0, NULL, SDL_FLIP_NONE);
}
