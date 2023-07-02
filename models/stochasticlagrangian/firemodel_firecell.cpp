//
// Created by nex on 10.06.23.
//

#include "firemodel_firecell.h"

FireCell::FireCell(int x, int y, std::mt19937 gen, FireModelParameters &parameters, int raster_value) : parameters_(parameters) {
    burningDuration_ = parameters_.GetCellBurningDuration();
    tau_ign = parameters_.GetIgnitionDelayTime();
    tickingDuration_ = 0;
    surface_ = SDL_CreateRGBSurfaceWithFormat(0, 1, 1, 32, SDL_PIXELFORMAT_ARGB8888);
    cell_initial_state_ = CellState(raster_value);
    cell_state_ = CellState(raster_value);
    cell_ = GetCell();
    x_ = x * parameters_.GetCellSize();
    y_ = y * parameters_.GetCellSize();

    // TODO Auslagern der Zufallszahlen in eine eigene Klasse?
    // Initialize random number generator
    gen_ = gen;
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
    VirtualParticle particle(x_ + (parameters_.GetCellSize() * 0.5), y_ + (parameters_.GetCellSize() * 0.5), parameters_.GetTauMemVirt(), parameters_.GetYStVirt(),
                             parameters_.GetYLimVirt(), parameters_.GetFlVirt(), parameters_.GetC0Virt(),
                             parameters_.GetLt(), gen_);

    return particle;
}

RadiationParticle FireCell::EmitRadiationParticle() {
    RadiationParticle radiation_particle(x_ + (parameters_.GetCellSize() * 0.5), y_ + (parameters_.GetCellSize() * 0.5), parameters_.GetLr(), parameters_.GetSf0(),
                                         parameters_.GetYStRad(),parameters_.GetYLimRad(), gen_);

    return radiation_particle;
}

void FireCell::Tick() {
    //throw an error if cellState ignited cells do not tick
    if (cell_state_ == GENERIC_BURNING || cell_state_ == GENERIC_BURNED) {
        throw std::runtime_error("FireCell::Tick() called on a cell that is not unburned");
    }
    tickingDuration_ += parameters_.GetDt();
}

void FireCell::burn() {
    burningDuration_ -= parameters_.GetDt();
    if (burningDuration_ <= 0) {
        SetCellState(GENERIC_BURNED);
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
    SetCellState(cell_initial_state_);
}

void FireCell::SetCellState(CellState cell_state) {
    cell_state_ = cell_state;
    delete cell_;
    cell_ = GetCell();
}

void FireCell::ShowInfo() {
    ImVec4 color = cell_->GetImVecColor();
    ImGui::ColorButton("MyColor##3", {color.x / 255, color.y / 255, color.z / 255, color.w / 255}, ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoPicker);
    ImGui::SameLine();
    ImGui::TextUnformatted(CellStateToString(cell_state_).c_str());
    ImGui::Text("Burning duration: %f", burningDuration_);
    ImGui::Text("Ticking duration: %f", tickingDuration_);
    ImGui::Text("Tau ign: %f", tau_ign);
}

Uint32 FireCell::GetMappedColor() {
    return cell_->GetMappedColor();
}

FireCell::~FireCell() {
    delete cell_;
    SDL_FreeSurface(surface_);
}
