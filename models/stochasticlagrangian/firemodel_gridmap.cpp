//
// Created by nex on 08.06.23.
//

#include "firemodel_gridmap.h"

GridMap::GridMap(Wind* wind, FireModelParameters &parameters) : parameters_(parameters) {
    rows_ = parameters_.GetGridNx(); //in cells
    cols_ = parameters_.GetGridNy(); //in cells
    wind_ = wind;

    cells_ = std::vector<std::vector<FireCell*>>(rows_, std::vector<FireCell*>(cols_, nullptr));
    for (int x = 0; x < rows_; ++x) {
        for (int y = 0; y < cols_; ++y) {
            cells_[x][y] = new FireCell(x, y, parameters_);  // Note the use of 'new' here.
        }
    }
}

int GridMap::GetRows() {
    return rows_;
}

int GridMap::GetCols() {
    return cols_;
}

void GridMap::UpdateVirtualParticles(std::unordered_set<Point>& visited_cells) {
    // Update virtual particles
    for (auto &particle : virtual_particles_) {
        particle.UpdateState(*wind_, parameters_.GetDt());

        double x, y;
        particle.GetPosition(x, y);
        int i, j;
        parameters_.ConvertRealToGridCoordinates(x, y, i, j);
        // check if particle is still in the grid
        if (i < 0 || i >= rows_ || j < 0 || j >= cols_) {
            // Particle is outside the grid, so it is no longer visited
            particle.RemoveParticle();
            continue;
        }
        Point p = Point(i, j);
        // Add particle to visited cells
        visited_cells.insert(p);

        if (particle.IsCapableOfIgnition() && cells_[p.x_][p.y_]->GetIgnitionState() == UNBURNED) {
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
}

void GridMap::UpdateRadiationParticles(std::unordered_set<Point> &visited_cells) {
    for (auto &particle : radiation_particles_) {
        particle.UpdateState(parameters_.GetDt());

        double x, y;
        particle.GetPosition(x, y);
        int i, j;
        parameters_.ConvertRealToGridCoordinates(x, y, i, j);
        // check if particle is still in the grid
        if (i < 0 || i >= rows_ || j < 0 || j >= cols_) {
            // Particle is outside the grid, so it is no longer visited
            particle.RemoveParticle();
            continue;
        }
        Point p = Point(i, j);
        // Add particle to visited cells
        visited_cells.insert(p);
        if (particle.IsCapableOfIgnition() && cells_[p.x_][p.y_]->GetIgnitionState() == UNBURNED) {
            ticking_cells_.insert(p);
        }
    }
    // Remove particles that are not capable of burning cells
    radiation_particles_.erase(
            std::remove_if(radiation_particles_.begin(), radiation_particles_.end(),
                           [](const RadiationParticle& particle) {
                               return !particle.IsCapableOfIgnition();
                           }),
            radiation_particles_.end());
}


void GridMap::UpdateParticles() {
    // Track cells visited by particles
    std::unordered_set<Point> visited_cells;

    UpdateVirtualParticles(visited_cells);
    UpdateRadiationParticles(visited_cells);

    // Check each cell in ticking_cells_ if it is currently visited
    for (auto it = ticking_cells_.begin(); it != ticking_cells_.end(); ) {
        if (visited_cells.find(*it) == visited_cells.end()) {
            // The cell is not currently visited, so it is no longer ticking
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
    VirtualParticle virtual_particle = cells_[x][y]->EmitVirtualParticle();
    RadiationParticle radiation_particle = cells_[x][y]->EmitRadiationParticle();
    virtual_particles_.push_back(virtual_particle);
    radiation_particles_.push_back(radiation_particle);
    burning_cells_.insert(Point(x, y));
}

void GridMap::UpdateCells() {
    // Iterate over ticking cells and ignite them if their ignition time has come
    for (auto it = ticking_cells_.begin(); it != ticking_cells_.end(); ) {
        int x = it->x_;
        int y = it->y_;
        cells_[x][y]->Tick();
        if (cells_[x][y]->ShouldIgnite()) {
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
        cells_[x][y]->burn();
        if (cells_[x][y]->GetIgnitionState() == CellState::BURNED) {
            // The cell has burned out, so it is no longer burning
            it = burning_cells_.erase(it);
        } else {
            ++it;
        }
    }
}

int GridMap::GetNumParticles() {
    //Get the number of virtual particles
    return (virtual_particles_.size() + radiation_particles_.size());
}

void GridMap::ExtinguishCell(int x, int y) {
    cells_[x][y]->Extinguish();
}

CellState GridMap::GetCellState(int x, int y) {
    return cells_[x][y]->GetIgnitionState();
}

void GridMap::ShowCellInfo(int x, int y) {
    cells_[x][y]->ShowInfo();
}
