//
// Created by nex on 10.06.23.
//

#include "firemodel_firecell.h"

FireCell::FireCell(int x, int y, FireModelParameters &parameters) : parameters_(parameters) {
    burningDuration_ = parameters_.GetCellBurningDuration();
    ignitionDelay_ = parameters_.GetCellIgnitionThreshold();
    tickingDuration_ = 0;
    cell_state_ = UNBURNED; // 0 = unburned, 1 = burning, 2 = burned
    x_ = x;
    y_ = y;

    // TODO Auslagern der Zufallszahlen in eine eigene Klasse?
    // Initialize random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.1, 0.2);
    std::uniform_int_distribution<> sign_dis(-1, 1);
    ignitionDelay_ += sign_dis(gen) * ignitionDelay_ * dis(gen);
}

CellState FireCell::GetIgnitionState() {
    return cell_state_;
}

void FireCell::Ignite() {
    if (cell_state_ != UNBURNED) {
        throw std::runtime_error("FireCell::Ignite() called on a cell that is not unburned");
    }
    cell_state_ = BURNING;
}

VirtualParticle FireCell::EmitVirtualParticle() {
    VirtualParticle particle(x_, y_, parameters_.GetTauMemVirt(), parameters_.GetYStVirt(),
                             parameters_.GetYLimVirt(), parameters_.GetFlVirt(), parameters_.GetC0Virt(),
                             parameters_.GetLt());

    return particle;
}

RadiationParticle FireCell::EmitRadiationParticle() {
    RadiationParticle radiation_particle(x_, y_, parameters_.GetLr(), parameters_.GetSf0(),
                                         parameters_.GetYStRad(),parameters_.GetYLimRad());

    return radiation_particle;
}

void FireCell::Tick() {
    //throw an error if cellState ignited cells do not tick
    if (cell_state_ == BURNING || cell_state_ == BURNED) {
        throw std::runtime_error("FireCell::Tick() called on a cell that is not unburned");
    }

    tickingDuration_ += parameters_.GetDt();
}

void FireCell::burn() {
    burningDuration_ -= parameters_.GetDt();
    if (burningDuration_ <= 0) {
        cell_state_ = BURNED;
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
    // TODO useful?
    cell_state_ = BURNED;
}
