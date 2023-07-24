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

void DroneRenderer::Render(std::pair<int, int> position, int size, int view_range, double angle) {
    // Render the arrow
    SDL_Rect destRect = {position.first, position.second, size, size}; // x, y, width and height of the arrow
    SDL_RenderCopyEx(renderer_, drone_texture_, NULL, &destRect, angle * 180 / M_PI, NULL, SDL_FLIP_NONE);
    SDL_Rect view_range_rect = {position.first - (view_range * size) / 2, position.second - (view_range * size) / 2, view_range * size, view_range * size};
    SDL_SetRenderDrawColor(renderer_, 255, 0, 0, 255);
    SDL_RenderDrawRect(renderer_, &view_range_rect);
}
