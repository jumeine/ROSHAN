//
// Created by nex on 27.06.23.
//

#ifndef ROSHAN_FIREMODEL_CELL_INTERFACE_H
#define ROSHAN_FIREMODEL_CELL_INTERFACE_H

#include <SDL.h>
#include "imgui.h"
#include "utils.h"

class ICell {
public:
    virtual ~ICell() = default;
    ImVec4 GetImVecColor() {
        return ImVec4(color_.r, color_.g, color_.b, color_.a);
    }
    Uint32 GetMappedColor() { return mapped_color_; }
    double GetCellBurningDuration() { return cell_burning_duration_; }
    double GetIgnitionDelayTime() { return ignition_delay_time_; }
    double GetSf0Mean() { return radiation_sf0_[0]; }
    double GetSf0Std() { return radiation_sf0_[1]; }
    int GetNumParticles() { return num_particles_; }
    void SetCellBurningDuration(double cell_burning_duration) { cell_burning_duration_ = cell_burning_duration; }

protected:
    SDL_Color color_;
    Uint32 mapped_color_;
    double cell_burning_duration_;
    double ignition_delay_time_;
    double radiation_sf0_[2]; // No wind propagation speed of radiation particles [m/s] (mean and std)
    int num_particles_;

};

#endif //ROSHAN_FIREMODEL_CELL_INTERFACE_H
