//
// Created by nex on 11.06.23.
//

#include "virtual_particle.h"

VirtualParticle::VirtualParticle(int x, int y, double tau_mem) {
    for (int i = 0; i < 2; ++i) {
        U_[i] = 0.0;
    }
    X_[0] = x;
    X_[1] = y;
    Y_st_ = 1.0;
    tau_mem_ = tau_mem; // A few tens of seconds
    Y_lim_ = 0.2;
    Fl_ = 1.0;
    C0_ = 2.0;
}

void VirtualParticle::UpdateState(double u_prime, double Lt, double Uw_i, double dt) {
    // Update Y_st
    Y_st_ -= Y_st_ / tau_mem_ * dt;

    // Update X and U for each direction
    for (int i = 0; i < 2; ++i) {
        // Generate a normally-distributed random number for N_i
        double N_i = ((double) rand() / (RAND_MAX)) * 2.0 - 1.0; // this is a simplification

        // Update U
        U_[i] += -(((2.0 + 3.0 * C0_) / 4.0) * (u_prime / Lt) * (U_[i] - Uw_i) * dt) + pow((C0_ * pow(u_prime, 3) / Lt * dt), 0.5) * N_i;

        // Update X
        X_[i] += Fl_ * U_[i] * dt;
    }
}

bool VirtualParticle::IsCapableOfIgnition() const {
    return Y_st_ >= Y_lim_;
}

void VirtualParticle::GetPosition(double &x1, double &x2) const {
    x1 = X_[0];
    x2 = X_[1];
}

void VirtualParticle::RemoveParticle() {
    Y_st_ = 0.0;
}
