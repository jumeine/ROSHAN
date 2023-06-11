//
// Created by nex on 08.06.23.
//

#include "firemodel_gridmap.h"

GridMap::GridMap(int rows, int cols, Wind* wind) {
    rows_ = rows;
    cols_ = cols;
    wind_ = wind;

    cells_ = std::vector<std::vector<FireCell*>>(rows_, std::vector<FireCell*>(cols_, nullptr));
    for (int i = 0; i < rows_; ++i) {
        for (int j = 0; j < cols_; ++j) {
            cells_[i][j] = new FireCell(wind_, 0.1, i, j);  // Note the use of 'new' here.
        }
    }
}

int GridMap::GetRows() {
    return rows_;
}

int GridMap::GetCols() {
    return cols_;
}

void GridMap::UpdateState(double Lt, double dt) {
    double u_prime = wind_->GetTurbulece();
    double Uw_i = wind_->GetWindSpeed();
    UpdateParticles(u_prime, Lt, Uw_i, dt);
}

void GridMap::UpdateParticles(double u_prime, double Lt, double Uw_i, double dt) {
    std::unordered_set<Point> visited_cells;
    // Update virtual particles
    for (auto &particle : virtual_particles_) {
        particle.UpdateState(u_prime, Lt, Uw_i, dt);

        double x, y;
        particle.GetPosition(x, y);
        // check if particle is still in the grid
        if (x < 0 || x >= rows_ || y < 0 || y >= cols_) {
            // Particle is outside the grid, so it is no longer visited
            particle.RemoveParticle();
            continue;
        }
        Point p = Point(int(x), int(y));
        visited_cells.insert(p);

        if (particle.IsCapableOfIgnition() && !cells_[p.x_][p.y_]->GetIgnitionState()) {
            ticking_cells_.insert(p);
        }
    }
    // Remove particles that are not capable of burning cells
    virtual_particles_.erase(
            std::remove_if(virtual_particles_.begin(), virtual_particles_.end(),
                           [](const VirtualParticle& particle) {
                               return !particle.IsCapableOfIgnition();
                           }),
            virtual_particles_.end());

    // Check each cell in ticking_cells_ if it is currently visited
    for (auto it = ticking_cells_.begin(); it != ticking_cells_.end(); ) {
        if (visited_cells.find(*it) == visited_cells.end()) {
            // The cell is not currently visited, so it is no longer ticking
            it = ticking_cells_.erase(it);
        } else {
            ++it;
        }
    }

    // Update radiation particles TODO implement radiation particles
    for (auto &particle : radiation_particles_) {
        particle.UpdateState(dt);
    }
}

void GridMap::HandleInteractions(double dt) {
    std::vector<std::pair<int, int>> toIgnite;

    // Check which cells should be ignited
//    for (int y = 0; y < cols_; ++y) {
//        for (int x = 0; x < rows_; ++x) {
//            if (cells_[x][y].CanIgniteOther()) {
//                // Try to ignite neighboring cells
//                for (int dy = -1; dy <= 1; ++dy) {
//                    for (int dx = -1; dx <= 1; ++dx) {
//                        int ny = y + dy;
//                        int nx = x + dx;
//                        if (ny >= 0 && ny < cols_ && nx >= 0 && nx < rows_) {
//                            toIgnite.push_back({nx, ny});
//                        }
//                    }
//                }
//            }
//        }
    }

GridMap::~GridMap() {
    for (auto& row : cells_) {
        for (auto& cell : row) {
            delete cell;
        }
    }
}

void GridMap::IgniteCell(int i, int j, double dt) {
    VirtualParticle particle = cells_[i][j]->Ignite(dt);
    virtual_particles_.push_back(particle);
    burning_cells_.insert(Point(i, j));
}

void GridMap::UpdateCells(double dt) {
    // Iterate over ticking cells and ignite them if their ignition time has come
    for (auto it = ticking_cells_.begin(); it != ticking_cells_.end(); ) {
        int x = it->x_;
        int y = it->y_;
        cells_[x][y]->Tick(dt);
        if (cells_[x][y]->ShouldIgnite()) {
            // The cell has ignited, so it is no longer ticking
            it = ticking_cells_.erase(it);
            // Ignite the cell
            IgniteCell(x, y, dt);
        } else {
            ++it;
        }
    }
    // Iterate over burning cells and let them burn
    for (auto it = burning_cells_.begin(); it != burning_cells_.end(); ) {
        int x = it->x_;
        int y = it->y_;
        cells_[x][y]->burn(dt);
        if (!cells_[x][y]->GetIgnitionState()) {
            // The cell has burned out, so it is no longer burning
            it = burning_cells_.erase(it);
        } else {
            ++it;
        }
    }
}

int GridMap::GetNumParticles() {
    //Get the number of virtual particles
    return virtual_particles_.size();
}

void GridMap::SetTauMem(double tau_mem) {
    // Every new created virtual particle will have this tauMem
    for (auto &cell_row : cells_) {
        for (auto &cell : cell_row) {
            cell->SetTauMem(tau_mem);
        }
    }
}
