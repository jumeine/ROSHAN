//
// Created by nex on 10.06.23.
//

#include <iostream>
#include "firemodel_firecell.h"

FireCell::FireCell(int x, int y, std::mt19937 gen, FireModelParameters &parameters, int raster_value) : parameters_(parameters) {
    surface_ = SDL_CreateRGBSurfaceWithFormat(0, 1, 1, 32, SDL_PIXELFORMAT_ARGB8888);

    //Cell State
    cell_initial_state_ = CellState(raster_value);
    cell_state_ = CellState(raster_value);
    cell_ = GetCell();
    mother_cell_ = GetCell();

    // Cell Parameters
    x_ = x * parameters_.GetCellSize();
    y_ = y * parameters_.GetCellSize();
    ticking_duration_ = 0;

    num_particles_ = mother_cell_->GetNumParticles();
//    num_particles_ = 5;

    if (parameters_.map_is_uniform_) {
        burning_duration_ = parameters_.GetCellBurningDuration();
        tau_ign = parameters_.GetIgnitionDelayTime();
    } else {
        burning_duration_ = cell_->GetCellBurningDuration();
        tau_ign = cell_->GetIgnitionDelayTime();
    }

    particle_emission_threshold_ = (burning_duration_ - 1) / num_particles_;

    // TODO Auslagern der Zufallszahlen in eine eigene Klasse?
    // Initialize random number generator
    std::random_device rd;
    gen_ = gen;
    gen_.seed(rd());
    std::uniform_real_distribution<> dis(0.1, 0.2);
    std::uniform_int_distribution<> sign_dis(-1, 1);
    tau_ign += sign_dis(gen_) * tau_ign * dis(gen_);
}

CellState FireCell::GetIgnitionState() {
    return cell_state_;
}

ICell *FireCell::GetCell() {
    // Create switch statement for each cell state in CellState enum
    ICell *cell_;
    switch (cell_state_) {
        case GENERIC_UNBURNED:
            cell_ = new CellGenericUnburned(surface_->format);
            break;
        case GENERIC_BURNING:
            cell_ = new CellGenericBurning(surface_->format);
            break;
        case GENERIC_BURNED:
            cell_ = new CellGenericBurned(surface_->format);
            break;
        case LICHENS_AND_MOSSES:
            cell_ = new CellLichensAndMosses(surface_->format);
            break;
        case LOW_GROWING_WOODY_PLANTS:
            cell_ = new CellLowGrowingWoodyPlants(surface_->format);
            break;
        case NON_AND_SPARSLEY_VEGETATED:
            cell_ = new CellNonAndSparsleyVegetated(surface_->format);
            break;
        case OUTSIDE_AREA:
            cell_ = new CellOutsideArea(surface_->format);
            break;
        case PERIODICALLY_HERBACEOUS:
            cell_ = new CellPeriodicallyHerbaceous(surface_->format);
            break;
        case PERMANENT_HERBACEOUS:
            cell_ = new CellPermanentHerbaceous(surface_->format);
            break;
        case SEALED:
            cell_ = new CellSealed(surface_->format);
            break;
        case SNOW_AND_ICE:
            cell_ = new CellSnowAndIce(surface_->format);
            break;
        case WATER:
            cell_ = new CellWater(surface_->format);
            break;
        case WOODY_BROADLEAVED_DECIDUOUS_TREES:
            cell_ = new CellWoodyBroadleavedDeciduousTrees(surface_->format);
            break;
        case WOODY_BROADLEAVED_EVERGREEN_TREES:
            cell_ = new CellWoodyBroadleavedEvergreenTrees(surface_->format);
            break;
        case WOODY_NEEDLE_LEAVED_TREES:
            cell_ = new CellWoodyNeedleLeavedTrees(surface_->format);
            break;
        default:
            throw std::runtime_error("FireCell::GetCell() called on a celltype that is not defined");
    }
    return cell_;
}

void FireCell::Ignite() {
    if (cell_state_ == GENERIC_BURNING || cell_state_ == GENERIC_BURNED) {
        throw std::runtime_error("FireCell::Ignite() called on a cell that is not unburned");
    }
    SetCellState(GENERIC_BURNING);
}

VirtualParticle FireCell::EmitVirtualParticle() {
    std::random_device rd;
    gen_.seed(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    double x_pos_rnd = dis(gen_);
    double y_pos_rnd = dis(gen_);
    VirtualParticle particle(x_ + (parameters_.GetCellSize() * x_pos_rnd), y_ + (parameters_.GetCellSize() * y_pos_rnd), parameters_.GetTauMemVirt(), parameters_.GetYStVirt(),
                             parameters_.GetYLimVirt(), parameters_.GetFlVirt(), parameters_.GetC0Virt(),
                             parameters_.GetLt(), gen_);

    return particle;
}

RadiationParticle FireCell::EmitRadiationParticle() {
    std::random_device rd;
    gen_.seed(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    double x_pos_rnd = dis(gen_);
    double y_pos_rnd = dis(gen_);
    double x_pos = x_ + (parameters_.GetCellSize() * x_pos_rnd);
    double y_pos = y_ + (parameters_.GetCellSize() * y_pos_rnd);
    RadiationParticle radiation_particle(x_pos, y_pos, parameters_.GetLr(), mother_cell_->GetSf0Mean(), mother_cell_->GetSf0Std(),
                                         parameters_.GetYStRad(), parameters_.GetYLimRad(), gen_);

    return radiation_particle;
}

void FireCell::Tick() {
    //throw an error if cellState ignited cells do not tick
    if (cell_state_ == GENERIC_BURNING || cell_state_ == GENERIC_BURNED) {
        throw std::runtime_error("FireCell::Tick() called on a cell that is not unburned");
    }
    if (tau_ign != -1) {
        ticking_duration_ += parameters_.GetDt();
    }
}

bool FireCell::EmitNextParticle() {
    // Converting the burning_tick_ to an int before the modulo operation
    int burning_tick_int = static_cast<int>(burning_tick_);

    // Check if the last burning duration is not equal to the current burning tick
    bool is_new_burning_tick = ceil(last_burning_duration_) != floor(burning_tick_);

    // It's a new burning tick & the burning tick is a multiple of the particle emission threshold
    bool emit_condition = is_new_burning_tick && (burning_tick_int % particle_emission_threshold_ == 0);

    return emit_condition;
};

bool FireCell::burn() {
    last_burning_duration_ = burning_tick_;
    burning_duration_ -= parameters_.GetDt();
    burning_tick_ += parameters_.GetDt();
    if (burning_duration_ <= 0) {
        SetCellState(GENERIC_BURNED);
        return false;
    } else {
        return EmitNextParticle();
    }
}

bool FireCell::CanIgnite() {
    if (cell_state_ == GENERIC_BURNING || cell_state_ == GENERIC_BURNED  ||
        cell_state_ == SNOW_AND_ICE || cell_state_ == OUTSIDE_AREA || cell_state_ == WATER ||
        cell_state_ == NON_AND_SPARSLEY_VEGETATED) {
        return false;
    }
    return true;
}

bool FireCell::ShouldIgnite() {
    if (ticking_duration_ >= tau_ign) {
        ticking_duration_ = 0;
        return true;
    }
    return false;
}

void FireCell::Extinguish() {
    SetCellState(cell_initial_state_);
}

void FireCell::SetCellState(CellState cell_state) {
    cell_state_ = cell_state;
    delete cell_;
    cell_ = GetCell();
}

void FireCell::ShowInfo() {
    ImGui::Text("Current Cell State");
    ImVec4 color = cell_->GetImVecColor();
    ImGui::ColorButton("MyColor##3", {color.x / 255, color.y / 255, color.z / 255, color.w / 255}, ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoPicker);
    ImGui::SameLine();
    ImGui::TextUnformatted(CellStateToString(cell_state_).c_str());
    ImGui::Text("Mother Cell State");
    color = mother_cell_->GetImVecColor();
    ImGui::ColorButton("MyColor##4", {color.x / 255, color.y / 255, color.z / 255, color.w / 255}, ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoPicker);
    ImGui::SameLine();
    ImGui::TextUnformatted(CellStateToString(cell_initial_state_).c_str());
    ImGui::Text("%0.f m x %0.f m", parameters_.GetCellSize(), parameters_.GetCellSize());
    ImGui::Text("Burning duration: %.2f", burning_duration_);
    ImGui::Text("Ticking duration: %.2f", ticking_duration_);
    ImGui::Text("Tau ign: %.2f", tau_ign);
}

Uint32 FireCell::GetMappedColor() {
    return cell_->GetMappedColor();
}

FireCell::~FireCell() {
    delete cell_;
    SDL_FreeSurface(surface_);
}
