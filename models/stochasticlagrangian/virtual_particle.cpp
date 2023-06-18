//
// Created by nex on 11.06.23.
//

#include "virtual_particle.h"

VirtualParticle::VirtualParticle(int x, int y, double tau_mem, double Y_st,
                                 double Y_lim, double Fl, double C0, double Lt, std::mt19937 gen){
    for (int i = 0; i < 2; ++i) {
        U_[i] = 0.0;
    }
    X_[0] = x;
    X_[1] = y;
    tau_mem_ = tau_mem;
    Y_st_ = Y_st;
    Y_lim_ = Y_lim;
    Fl_ = Fl;
    C0_ = C0;
    Lt_ = Lt;

    gen_ = gen;
}

void VirtualParticle::UpdateState(Wind wind, double dt) {
    // Update Y_st
    Y_st_ -= Y_st_ / tau_mem_ * dt;
    double u_prime = wind.GetCurrentTurbulece();
    std::vector<double> Uw_i = {wind.getWindSpeedComponent1(), wind.getWindSpeedComponent2()};

    // Update X and U for each direction
    for (int i = 0; i < 2; ++i) {
        // Generate a normally-distributed random number for N_i
        std::normal_distribution<> d(0,1);
        double N_i = d(gen_);

        // Update U
        U_[i] += -(((2.0 + 3.0 * C0_) / 4.0) * (u_prime / Lt_) *
                (U_[i] - Uw_i[i]) * dt) +
                        pow((C0_ * pow(u_prime, 3) / Lt_ * dt), 0.5) * N_i;

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
