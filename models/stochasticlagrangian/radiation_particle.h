//
// Created by nex on 11.06.23.
//

#ifndef ROSHAN_RADIATION_PARTICLE_H
#define ROSHAN_RADIATION_PARTICLE_H

#include <cmath>

class RadiationParticle {

public:
    RadiationParticle(double x1, double x2, double angle, double status, double decayTimescale);
    void UpdateState(double dt);
    bool IsCapableOfIgnition() const { return Y_st_ >= Y_lim_; }

private:

    double X_[2];         // Position
    double phi_r_;        // Angle
    double Y_st_;         // Burning status
    double Y_lim_;        // Ignition limit
    double tau_mem_;      // Decay timescale
    double Lr_;           // Scaling factor for new position

};


#endif //ROSHAN_RADIATION_PARTICLE_H
