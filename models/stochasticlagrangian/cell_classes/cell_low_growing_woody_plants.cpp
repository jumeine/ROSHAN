//
// Created by nex on 27.06.23.
//

#include "../firemodel_cell_interface.h"
#include <SDL.h>

class CellLowGrowingWoodyPlants : public ICell {
public:
    CellLowGrowingWoodyPlants(SDL_PixelFormat* format) {
        color_ = {105, 76, 51, 255};
        mapped_color_ = SDL_MapRGBA(format, color_.r, color_.g, color_.b, color_.a);
        cell_burning_duration_ = 720;
        ignition_delay_time_ = 80;
        radiation_sf0_[0] = 0.06;
        radiation_sf0_[1] = 0.01;
        num_particles_ = 40;
    }
};