//
// Created by nex on 27.06.23.
//

#include "../firemodel_cell_interface.h"
#include <SDL.h>

class CellPermanentHerbaceous : public ICell {
public:
    CellPermanentHerbaceous(SDL_PixelFormat* format) {
        color_ = {250, 218, 94, 255};
        mapped_color_ = SDL_MapRGBA(format, color_.r, color_.g, color_.b, color_.a);
        cell_burning_duration_ = 360;
        ignition_delay_time_ = 60;
        radiation_sf0_[0] = 0.06;
        radiation_sf0_[1] = 0.02;
        num_particles_ = 20;
    }
};