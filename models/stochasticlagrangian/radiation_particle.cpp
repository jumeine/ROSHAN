//
// Created by nex on 11.06.23.
//

#include "radiation_particle.h"

// TODO Implementation of old Paper, implement both?
//RadiationParticle::RadiationParticle(double x, double y, double Lr, double Sf_0, double Y_st, double Y_lim) {
//    X_[0] = x;
//    X_[1] = y;
//    // Generate a normally-distributed random number for phi_r
//    double angle = ((double) rand() / (RAND_MAX)) * 2.0 * M_PI;
//    phi_r_ = angle;
//    Lr_ = Lr; // in meters (m)
//    Sf_0_ = Sf_0; // in meters per second (m/s)
//    tau_mem_ = Lr_ / Sf_0_; // in seconds (s)
//    Y_st_ = Y_st;
//    Y_lim_ = Y_lim;
//}
//
//void RadiationParticle::UpdateState(double dt) {
//    // Update position
//    X_[0] += (Sf_0_ * cos(phi_r_)) * dt;
//    X_[1] += (Sf_0_ * sin(phi_r_)) * dt;
//
//    // Update burning status
//    Y_st_ -= Y_st_ / tau_mem_ * dt;
//}
//
//void RadiationParticle::GetPosition(double &x1, double &x2) const {
//    x1 = X_[0];
//    x2 = X_[1];
//}
//
//void RadiationParticle::RemoveParticle() {
//    Y_st_ = 0.0;
//}

RadiationParticle::RadiationParticle(double x, double y, double Lr, double Sf_0, double Y_st, double Y_lim, std::mt19937 gen) {
    X_[0] = x;
    X_[1] = y;
    X_mc_[0] = x;
    X_mc_[1] = y;

    phi_p_ = 0.0;
    r_p_ = 0.0;
    gen_ = gen;

    Sf_0_mean_ = Sf_0;
    Sf_0_std_ = 1;

    std::normal_distribution<> d(Sf_0_mean_, Sf_0_std_);

    Sf_0_ = d(gen_);

    if (Sf_0_ < 0) {
        Sf_0_ = -Sf_0_;
    } // Wind Speed can't be negative

    Lr_ = Lr; // Characteristic radiation length in meters (m)
    tau_mem_ = Lr_ / Sf_0_; // in seconds (s)
    Y_st_ = Y_st;
    Y_lim_ = Y_lim;
}

void RadiationParticle::UpdateState(double dt) {

    std::normal_distribution<> d(0,1);

    double N_phi = d(gen_);
    double N_r = d(gen_);

    // Update phi_p
    phi_p_ += 2 * M_PI * N_phi;
    r_p_ += Sf_0_mean_ * Sf_0_std_ * sqrt(dt) * N_r;

    // Update position
    X_[0] = X_mc_[0] + r_p_ * cos(phi_p_);
    X_[1] = X_mc_[1] + r_p_ * sin(phi_p_);

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