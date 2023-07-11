//
// Created by nex on 08.06.23.
//

#include "firemodel_gridmap.h"

GridMap::GridMap(Wind* wind, FireModelParameters &parameters,
                 std::vector<std::vector<int>>* rasterData) : parameters_(parameters) {
    cols_ = rasterData->size();
    rows_ = (rasterData->empty()) ? 0 : (*rasterData)[0].size();
    wind_ = wind;

    // Generate a normally-distributed random number for phi_r
    gen_ = std::mt19937(rd_());

    cells_ = std::vector<std::vector<FireCell*>>(cols_, std::vector<FireCell*>(rows_, nullptr));
    for (int x = 0; x < cols_; ++x) {
        for (int y = 0; y < rows_; ++y) {
            cells_[x][y] = new FireCell(x, y, gen_, parameters_, (*rasterData)[x][y]);
        }
    }
    virtual_particles_.reserve(100000);
    radiation_particles_.reserve(100000);
    ticking_cells_.reserve(100000);
    burning_cells_.reserve(100000);
    changed_cells_.reserve(100000);
}

// Templated function to avoid repeating common code
template <typename ParticleType>
void GridMap::UpdateVirtualParticles(std::vector<ParticleType> &particles, std::vector<std::vector<bool>> &visited_cells) {
    for (auto it = particles.begin(); it != particles.end();) {

        if constexpr (std::is_same<ParticleType, VirtualParticle>::value) {
            it->UpdateState(*wind_, parameters_.GetDt());
        } else {
            it->UpdateState(parameters_.GetDt());
        }

        double x, y;
        it->GetPosition(x, y);
        int i, j;
        parameters_.ConvertRealToGridCoordinates(x, y, i, j);

        // check if particle is still in the grid
        if (!IsPointInGrid(i, j) || !it->IsCapableOfIgnition()) {
            // Particle is outside the grid, so it is no longer visited
            // OR it is not capable of ignition
            std::iter_swap(it, --particles.end());
            particles.pop_back();

            continue;
        }

        Point p = Point(i, j);

        // Add particle to visited cells, if not allready visited
        visited_cells[p.x_][p.y_] = true;

        // If cell can ignite, add particle to cell, if not allready in
        if (cells_[p.x_][p.y_]->CanIgnite()) {
            ticking_cells_.insert(p);
        }

        ++it;
    }
}

void GridMap::UpdateParticles() {
    std::vector<std::vector<bool>> visited_cells(cols_, std::vector<bool>(rows_, false));
    UpdateVirtualParticles(virtual_particles_, visited_cells);
    UpdateVirtualParticles(radiation_particles_, visited_cells);

    for (auto it = ticking_cells_.begin(); it != ticking_cells_.end(); ) {
        // If cell is not visited, remove it from ticking cells
        if (!visited_cells[it->x_][it->y_]) {
            it = ticking_cells_.erase(it);
        } else {
            ++it;
        }
    }
}

GridMap::~GridMap() {
    for (auto& row : cells_) {
        for (auto& cell : row) {
            delete cell;
        }
    }
}

void GridMap::IgniteCell(int x, int y) {
    cells_[x][y]->Ignite();
    burning_cells_.insert(Point(x, y));
    changed_cells_.emplace_back(x, y);
}

void GridMap::UpdateCells() {
    // Iterate over ticking cells and ignite them if their ignition time has come
    for (auto it = ticking_cells_.begin(); it != ticking_cells_.end(); ) {
        int x = it->x_;
        int y = it->y_;
        auto& cell = cells_[x][y];
        cell->Tick();
        if (cell->ShouldIgnite()) {
            // The cell has ignited, so it is no longer ticking
            it = ticking_cells_.erase(it);
            // Ignite the cell
            IgniteCell(x, y);
        } else {
            ++it;
        }
    }
    // Iterate over burning cells and let them burn
    for (auto it = burning_cells_.begin(); it != burning_cells_.end(); ) {
        int x = it->x_;
        int y = it->y_;
        auto& cell = cells_[x][y];
        bool cell_emits_particle = cell->burn();
        if (cell_emits_particle) {
            virtual_particles_.push_back(std::move(cell->EmitVirtualParticle()));
            radiation_particles_.push_back(std::move(cell->EmitRadiationParticle()));
            changed_cells_.emplace_back(x, y);
        }
        if (cell->GetIgnitionState() == CellState::GENERIC_BURNED) {
            // The cell has burned out, so it is no longer burning
            it = burning_cells_.erase(it);
            changed_cells_.push_back(Point(x, y));
        } else {
            ++it;
        }
    }
}

void GridMap::ExtinguishCell(int x, int y) {
    cells_[x][y]->Extinguish();
    burning_cells_.erase(Point(x, y));
    ticking_cells_.erase(Point(x, y));
    //Erase all particles that are on that cell
    std::function<void(double&, double&, int&, int&)> convertr =
            std::bind(&FireModelParameters::ConvertRealToGridCoordinates, &parameters_, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    virtual_particles_.erase(
            std::remove_if(virtual_particles_.begin(), virtual_particles_.end(),
                           [x, y, &convertr](const VirtualParticle& particle) {
                               double particle_x, particle_y;
                               particle.GetPosition(particle_x, particle_y);
                               int i, j;
                               convertr(particle_x, particle_y, i, j);
                               return i == x && j == y;
                           }),
            virtual_particles_.end());
    radiation_particles_.erase(
            std::remove_if(radiation_particles_.begin(), radiation_particles_.end(),
                           [x, y, &convertr](const RadiationParticle& particle) {
                               double particle_x, particle_y;
                               particle.GetPosition(particle_x, particle_y);
                               int i, j;
                               convertr(particle_x, particle_y, i, j);
                               return i == x && j == y;
                           }),
            radiation_particles_.end());

    changed_cells_.push_back(Point(x, y));
}
