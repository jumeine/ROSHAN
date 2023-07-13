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
        ignition_delay_time_ = 750;
        radiation_sf0_[0] = 0.1;
        radiation_sf0_[1] = 0.0075;
        num_convection_particles_ = 1000;
        radiation_length_min_ = 9;
        radiation_length_max_ = 10;
        num_radiation_particles_ = 55;
    }
};