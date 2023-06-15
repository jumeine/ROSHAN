//
// Created by nex on 10.06.23.
//

#ifndef ROSHAN_FIREMODEL_FIRECELL_H
#define ROSHAN_FIREMODEL_FIRECELL_H

#include "../../point.h"
#include "virtual_particle.h"
#include "radiation_particle.h"
#include "model_parameters.h"
#include "wind.h"
#include "utils.h"
#include <vector>
#include <random>
#include <chrono>

class FireCell {

public:
    FireCell(int x, int y, FireModelParameters &parameters);

    CellState GetIgnitionState();
    CellState GetCellState() { return cell_state_; }

    void Ignite();
    VirtualParticle EmitVirtualParticle();
    RadiationParticle EmitRadiationParticle();

    void Tick();
    void burn();
    bool ShouldIgnite();
    void Extinguish();
private:
    FireModelParameters &parameters_;

    double burningDuration_;
    double tickingDuration_;
    double tau_ign;
    int x_; // Start of the x coordinate in meters (m)
    int y_; // Start of the y coordinate in meters (m)
    CellState cell_state_;
};


#endif //ROSHAN_FIREMODEL_FIRECELL_H
