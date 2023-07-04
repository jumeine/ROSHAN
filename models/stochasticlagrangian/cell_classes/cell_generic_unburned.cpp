//
// Created by nex on 27.06.23.
//

#include "../firemodel_cell_interface.h"
#include <SDL.h>

class CellGenericUnburned : public ICell {
public:
    CellGenericUnburned(SDL_PixelFormat* format) {
        color_ = {50, 190, 75, 255};
        mapped_color_ = SDL_MapRGBA(format, color_.r, color_.g, color_.b, color_.a);
        cell_burning_duration_ = 80;
        ignition_delay_time_ = 10;
    }
};