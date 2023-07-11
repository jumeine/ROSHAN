//
// Created by nex on 11.06.23.
//

#include <iostream>
#include "radiation_particle.h"

RadiationParticle::RadiationParticle(double x, double y, double Lr, double Sf_0_mean, double Sf_0_std, double Y_st, double Y_lim, std::mt19937 gen) {
    X_[0] = x;
    X_[1] = y;
    X_mc_[0] = x;
    X_mc_[1] = y;

    phi_p_ = 0.0;
    r_p_ = 0.0;

    gen_ = gen;

    Sf_0_mean_ = Sf_0_mean;
    Sf_0_std_ = Sf_0_std;

    std::normal_distribution<> d(Sf_0_mean_, Sf_0_std_);
    normal_dist_ = std::normal_distribution<>(0,1);

    Sf_0_ = d(gen_);

    if (Sf_0_ < 0) {
        Sf_0_ = -Sf_0_;
    } // Wind Speed can't be negative

    Lr_ = Lr; // Characteristic radiation length in meters (m)
    if (Sf_0_ > 0.001) {
        tau_mem_ = Lr_ / Sf_0_; // in seconds (s)
    } else {
        tau_mem_ = Y_st;
    }
    Y_st_ = Y_st;
    Y_lim_ = Y_lim;
}

void RadiationParticle::UpdateState(double dt) {

    N_phi_ = normal_dist_(gen_);
    N_r_ = normal_dist_(gen_);

    // Update phi_p
    phi_p_ += M_PI_2_ * N_phi_;
    r_p_ += Sf_0_mean_ * dt +  Sf_0_std_ * sqrt(dt) * N_r_;

    // Update position
    X_[0] = X_mc_[0] + r_p_ * cos(phi_p_);
    X_[1] = X_mc_[1] + r_p_ * sin(phi_p_);

    // Update burning status
    Y_st_ -= Y_st_ / tau_mem_ * dt;
}