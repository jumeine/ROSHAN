//
// Created by nex on 10.06.23.
//

#include "firemodel_firecell.h"

FireCell::FireCell(int x, int y, FireModelParameters &parameters) : parameters_(parameters) {
    burningDuration_ = parameters_.GetCellBurningDuration();
    tau_ign = parameters_.GetIgnitionDelayTime();
    tickingDuration_ = 0;
    cell_state_ = UNBURNED; // 0 = unburned, 1 = burning, 2 = burned
    x_ = x * parameters_.GetCellSize();
    y_ = y * parameters_.GetCellSize();

    // TODO Auslagern der Zufallszahlen in eine eigene Klasse?
    // Initialize random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.1, 0.2);
    std::uniform_int_distribution<> sign_dis(-1, 1);
    tau_ign += sign_dis(gen) * tau_ign * dis(gen);
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
    VirtualParticle particle(x_ + (parameters_.GetCellSize() * 0.5), y_ + (parameters_.GetCellSize() * 0.5), parameters_.GetTauMemVirt(), parameters_.GetYStVirt(),
                             parameters_.GetYLimVirt(), parameters_.GetFlVirt(), parameters_.GetC0Virt(),
                             parameters_.GetLt());

    return particle;
}

RadiationParticle FireCell::EmitRadiationParticle() {
    RadiationParticle radiation_particle(x_ + (parameters_.GetCellSize() * 0.5), y_ + (parameters_.GetCellSize() * 0.5), parameters_.GetLr(), parameters_.GetSf0(),
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
    if (tickingDuration_ >= tau_ign) {
        tickingDuration_ = 0;
        return true;
    }
    return false;
}

void FireCell::Extinguish() {
    cell_state_ = BURNED;
}

void FireCell::ShowInfo() {
    ImGui::Text("Cell state: %d", cell_state_);
    ImGui::Text("Burning duration: %f", burningDuration_);
    ImGui::Text("Ticking duration: %f", tickingDuration_);
    ImGui::Text("Tau ign: %f", tau_ign);
}
