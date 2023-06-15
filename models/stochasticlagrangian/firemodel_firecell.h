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
    double ignitionDelay_;
    int x_;
    int y_;
    CellState cell_state_;
};


#endif //ROSHAN_FIREMODEL_FIRECELL_H
