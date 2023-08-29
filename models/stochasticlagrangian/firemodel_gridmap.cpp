//
// Created by nex on 08.06.23.
//

#include "firemodel_gridmap.h"

GridMap::GridMap(std::shared_ptr<Wind> wind, FireModelParameters &parameters,
                 std::vector<std::vector<int>>* rasterData) : parameters_(parameters) {
    cols_ = rasterData->size();
    rows_ = (rasterData->empty()) ? 0 : (*rasterData)[0].size();
    wind_ = wind;

    // Generate a normally-distributed random number for phi_r
    gen_ = std::mt19937(rd_());

    cells_ = std::vector<std::vector<std::shared_ptr<FireCell>>>(cols_, std::vector<std::shared_ptr<FireCell>>(rows_));
    for (int x = 0; x < cols_; ++x) {
        for (int y = 0; y < rows_; ++y) {
            cells_[x][y] = std::make_shared<FireCell>(x, y, gen_, parameters_, (*rasterData)[x][y]);
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
        cell->burn();
        bool cell_has_changed = false;
        auto should_emit_particle = cell->ShouldEmitNextParticles();
        if (parameters_.emit_convective_ && should_emit_particle.first) {
            virtual_particles_.push_back(std::move(cell->EmitConvectionParticle()));
            cell_has_changed = true;
        }
        if (parameters_.emit_radiation_ && should_emit_particle.second) {
            radiation_particles_.push_back(std::move(cell->EmitRadiationParticle()));
            cell_has_changed = true;
        }
        if (cell_has_changed)
            changed_cells_.emplace_back(x, y);

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
// Gets a view of the cells cell_type in a radius around the drone
std::pair<std::vector<std::vector<int>>, std::vector<std::vector<int>>> GridMap::GetDroneView(std::shared_ptr<DroneAgent> drone) {
    int drone_view_radius = drone->GetViewRange();
    std::pair<int, int> drone_position = drone->GetPosition();
    std::vector<std::vector<int>> cell_status(drone_view_radius + 1, std::vector<int>(drone_view_radius + 1, 0));
    std::vector<std::vector<int>> fire_status(drone_view_radius + 1, std::vector<int>(drone_view_radius + 1, 0));
    int drone_view_radius_2 = drone_view_radius / 2;
    for (int j = drone_position.second - drone_view_radius_2; j <= drone_position.second + drone_view_radius_2; ++j) {
        for (int i = drone_position.first - drone_view_radius_2; i <= drone_position.first + drone_view_radius_2; ++i) {
            int new_i = j - drone_position.second + drone_view_radius_2;
            int new_j = i - drone_position.first + drone_view_radius_2;
            if (IsPointInGrid(i, j)) {
                cell_status[new_i][new_j] = cells_[i][j]->GetCellInitialState();
                if (cells_[i][j]->IsBurning())
                    fire_status[new_i][new_j] = 1;
            } else {
                cell_status[new_i][new_j] = CellState::OUTSIDE_GRID;
            }
        }
    }
    return std::make_pair(cell_status, fire_status);
}
