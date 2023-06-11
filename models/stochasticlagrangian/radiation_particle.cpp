//
// Created by nex on 11.06.23.
//

#include "radiation_particle.h"

RadiationParticle::RadiationParticle(double x1, double x2, double angle, double status, double decayTimescale) {
    X_[0] = x1;
    X_[1] = x2;
    phi_r_ = angle;
    Y_st_ = status;
    tau_mem_ = decayTimescale;
    Lr_ = 10.0;
    Y_lim_ = 0.2;
}

void RadiationParticle::UpdateState(double dt) {
    // Update position
    X_[0] += Lr_ * cos(phi_r_);
    X_[1] += Lr_ * sin(phi_r_);

    // Update burning status
    Y_st_ -= Y_st_ / tau_mem_ * dt;

    // Ensure that the burning status remains within the valid range [0,1]
    if (Y_st_ < 0) Y_st_ = 0;
    if (Y_st_ > 1) Y_st_ = 1;
}
