//
// Created by nex on 10.06.23.
//

#include "firemodel_firecell.h"

FireCell::FireCell(Wind* wind, double Lt, int x, int y) {
    burningDuration_ = 100;
    ignitionDelay_ = 2;
    tickingDuration_ = 0;
    cell_state_ = 0; // 0 = unburned, 1 = burning, 2 = burned
    wind_ = wind;
    Lt_ = Lt;
    x_ = x;
    y_ = y;

    // Initialize random number generator
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<> dis(0.1, 0.2);
    std::uniform_int_distribution<> sign_dis(-1, 1);
    ignitionDelay_ += sign_dis(gen) * ignitionDelay_ * dis(gen);
}

bool FireCell::GetIgnitionState() {
    if(cell_state_ == 1) {
        return true;
    }
    return false;
}

VirtualParticle FireCell::Ignite(double dt) {
    cell_state_ = 1;
    VirtualParticle particle = EmitVirtualParticle(dt);

    return particle;
}

VirtualParticle FireCell::EmitVirtualParticle(double dt) {
    // Create a new particle
    double u_prime = wind_->GetTurbulece();
    double Uw_i = wind_->GetWindSpeed();
    VirtualParticle particle(x_, y_, tau_mem_);
    particle.UpdateState(u_prime, Lt_, Uw_i, dt);

    return particle;
}

RadiationParticle FireCell::EmitRadiationParticle(double x1, double x2, double angle, double status, double decay_timescale, double dt) {
    RadiationParticle radiation_particle(x1, x2, angle, status, decay_timescale);
    radiation_particle.UpdateState(dt);

    // Add the new radiation particle to the cell's collection
    return radiation_particle;
}

void FireCell::Tick(double dt) {
    //throw an error if cellState ignited cells do not tick
    if (cell_state_ == 1) {
        throw std::runtime_error("FireCell::tick() called on ignited cell");
    }

    tickingDuration_ += dt;
}

void FireCell::burn(double dt) {
    burningDuration_ -= dt;
    if (burningDuration_ <= 0) {
        cell_state_ = 2;
    }
}

bool FireCell::ShouldIgnite() {
    if (tickingDuration_ >= ignitionDelay_) {
        tickingDuration_ = 0;
        return true;
    }
    return false;
}

void FireCell::Extinguish() {
    cell_state_ = 2;
}

void FireCell::SetTauMem(double tau_mem) {
    tau_mem_ = tau_mem;
}
