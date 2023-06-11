//
// Created by nex on 10.06.23.
//

#ifndef ROSHAN_FIREMODEL_FIRECELL_H
#define ROSHAN_FIREMODEL_FIRECELL_H

#include "../../point.h"
#include "virtual_particle.h"
#include "radiation_particle.h"
#include "wind.h"
#include <vector>
#include <random>
#include <chrono>

class FireCell {

public:
    FireCell(Wind* wind, double Lt, int x, int y);

    bool GetIgnitionState();
    int GetCellState() { return cell_state_; }

    VirtualParticle Ignite(double dt);
    VirtualParticle EmitVirtualParticle(double dt);
    RadiationParticle EmitRadiationParticle(double x1, double x2, double angle, double status, double decayTimescale, double dt);

    void Tick(double dt);
    void burn(double dt);
    bool ShouldIgnite();
    void Extinguish();
    void SetTauMem(double tau_mem);
private:
    double burningDuration_;
    double tickingDuration_;
    double ignitionDelay_;
    double Lt_;
    double tau_mem_;
    int x_;
    int y_;
    int cell_state_;
    Wind* wind_;
};


#endif //ROSHAN_FIREMODEL_FIRECELL_H
