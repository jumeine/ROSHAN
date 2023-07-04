//
// Created by nex on 27.06.23.
//

#include "../firemodel_cell_interface.h"
#include <SDL.h>

class CellSealed : public ICell {
public:
    CellSealed(SDL_PixelFormat* format) {
        color_ = {100, 100, 100, 255};
        mapped_color_ = SDL_MapRGBA(format, color_.r, color_.g, color_.b, color_.a);
        cell_burning_duration_ = 15000;
        ignition_delay_time_ = 1500;
    }
};