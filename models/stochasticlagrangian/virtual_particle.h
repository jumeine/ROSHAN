//
// Created by nex on 11.06.23.
//

#ifndef ROSHAN_VIRTUAL_PARTICLE_H
#define ROSHAN_VIRTUAL_PARTICLE_H

#include <cmath>
#include <vector>
#include "wind.h"
#include "model_parameters.h"

class VirtualParticle {

public:

    VirtualParticle(int x, int y, double tau_mem, double Y_st,
                    double Y_lim, double Fl, double C0, double Lt);
    void UpdateState(Wind wind, double dt);
    void RemoveParticle();
    void GetPosition(double& x1, double& x2) const;
    bool IsCapableOfIgnition() const;

private:
    double X_[2]{};      //Position
    double U_[2]{};      //Belocity
    double Y_st_;        //Burning status
    double tau_mem_;     // Memory timescale
    double Y_lim_;       // Ignition limit
    double Fl_;          // Scaling factor for new position
    double C0_;          // A constant close to 2
    double Lt_;          // I dont really know what this is
};


#endif //ROSHAN_VIRTUAL_PARTICLE_H
