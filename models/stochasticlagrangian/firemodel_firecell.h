//
// Created by nex on 10.06.23.
//

#ifndef ROSHAN_FIREMODEL_FIRECELL_H
#define ROSHAN_FIREMODEL_FIRECELL_H

#include "../../point.h"
#include "virtual_particle.h"
#include "radiation_particle.h"
#include "model_parameters.h"
#include "imgui.h"
#include "wind.h"
#include "utils.h"
#include <vector>
#include <random>
#include <chrono>

class FireCell {

public:
    FireCell(int x, int y, std::mt19937 gen, FireModelParameters &parameters, int raster_value = 0);

    CellState GetIgnitionState();
    CellState GetCellState() { return cell_state_; }

    void Ignite();
    VirtualParticle EmitVirtualParticle();
    RadiationParticle EmitRadiationParticle();

    void Tick();
    void burn();
    bool ShouldIgnite();
    void Extinguish();
    void ShowInfo();
private:
    FireModelParameters &parameters_;

    double burningDuration_;
    double tickingDuration_;
    double tau_ign;
    int x_; // Start of the x coordinate in meters (m)
    int y_; // Start of the y coordinate in meters (m)
    CellState cell_state_;
    CellState cell_initial_state_;

    // Random Generator for the particles
    std::mt19937 gen_;

};


#endif //ROSHAN_FIREMODEL_FIRECELL_H
