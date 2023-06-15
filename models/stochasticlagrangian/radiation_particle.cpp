//
// Created by nex on 11.06.23.
//

#include "radiation_particle.h"

RadiationParticle::RadiationParticle(double x, double y, double Lr, double Sf_0, double Y_st, double Y_lim) {
    X_[0] = x;
    X_[1] = y;
    // Generate a normally-distributed random number for phi_r
    double angle = ((double) rand() / (RAND_MAX)) * 2.0 * M_PI;
    phi_r_ = angle;
    Lr_ = Lr;
    Sf_0_ = Sf_0;
    tau_mem_ = Lr_ / Sf_0_; // A few tens of seconds
    //TODO these values are hardcoded, change that
    Y_st_ = Y_st;
    Y_lim_ = Y_lim;
}

void RadiationParticle::UpdateState(double dt) {
    // Update position
    X_[0] += (Lr_ * cos(phi_r_)) * dt;
    X_[1] += (Lr_ * sin(phi_r_)) * dt;

    // Update burning status
    Y_st_ -= Y_st_ / tau_mem_ * dt;
}

void RadiationParticle::GetPosition(double &x1, double &x2) const {
    x1 = X_[0];
    x2 = X_[1];
}

void RadiationParticle::RemoveParticle() {
    Y_st_ = 0.0;
}