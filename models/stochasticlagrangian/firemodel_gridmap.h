//
// Created by nex on 08.06.23.
//

#ifndef ROSHAN_FIREMODEL_GRIDMAP_H
#define ROSHAN_FIREMODEL_GRIDMAP_H

#include <vector>
#include <unordered_set>
#include "../../point.h"
#include "../../point_hash.h"
#include "firemodel_firecell.h"
#include "model_parameters.h"
#include "wind.h"

class GridMap {
public:
    GridMap(Wind* wind, FireModelParameters &parameters);
    ~GridMap();

    int GetRows();
    int GetCols();
    void IgniteCell(int x, int y);
    void ExtinguishCell(int x, int y);
    CellState GetCellState(int x, int y);
    void UpdateVirtualParticles(std::unordered_set<Point>& visited_cells);
    void UpdateRadiationParticles(std::unordered_set<Point>& visited_cells);
    void UpdateParticles();
    void UpdateCells();
    int GetNumParticles();
    int GetNumCells() { return rows_ * cols_; }
    std::vector<VirtualParticle> GetVirtualParticles() const { return virtual_particles_; }
    std::vector<RadiationParticle> GetRadiationParticles() const { return radiation_particles_; }

    FireCell& At(int i, int j) {
        return *cells_[i][j];
    }

private:
    FireModelParameters &parameters_;
    Wind* wind_;
    int rows_; // Number of rows in the grid
    int cols_; // Number of columns in the grid
    double cell_size_; // Size of each cell in the grid in meters (m)
    std::vector<std::vector<FireCell*>> cells_;
    std::unordered_set<Point> ticking_cells_;
    std::unordered_set<Point> burning_cells_;

    std::vector<VirtualParticle> virtual_particles_;
    std::vector<RadiationParticle> radiation_particles_;
};


#endif //ROSHAN_FIREMODEL_GRIDMAP_H
