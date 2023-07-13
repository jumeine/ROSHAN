//
// Created by nex on 08.06.23.
//

#ifndef ROSHAN_FIREMODEL_GRIDMAP_H
#define ROSHAN_FIREMODEL_GRIDMAP_H

#include <vector>
#include <unordered_set>
#include "point.h"
#include "point_hash.h"
#include "firemodel_firecell.h"
#include "model_parameters.h"
#include "wind.h"
#include <random>
#include <iostream>
#include <memory>

class GridMap {
public:
    GridMap(std::shared_ptr<Wind> wind, FireModelParameters &parameters, std::vector<std::vector<int>>* rasterData = nullptr);
    ~GridMap();

    int GetRows() const { return rows_; }
    int GetCols() const { return cols_; }
    void IgniteCell(int x, int y);
    void ExtinguishCell(int x, int y);
    CellState GetCellState(int x, int y) { return cells_[x][y]->GetIgnitionState(); }
    void UpdateParticles();
    void UpdateCells();
    int GetNumParticles() { return virtual_particles_.size() + radiation_particles_.size();}
    void ShowCellInfo(int x, int y) { cells_[x][y]->ShowInfo(); }
    int GetNumCells() const { return rows_ * cols_; }
    std::vector<VirtualParticle> GetVirtualParticles() const { return virtual_particles_; }
    std::vector<RadiationParticle> GetRadiationParticles() const { return radiation_particles_; }
    std::vector<Point> GetChangedCells() const { return changed_cells_; }
    void ResetChangedCells() { changed_cells_.clear(); }

    FireCell& At(int i, int j) {
        return *cells_[i][j];
    }

private:
    FireModelParameters &parameters_;
    std::shared_ptr<Wind> wind_;
    int rows_; // Number of rows in the grid
    int cols_; // Number of columns in the grid
    std::vector<std::vector<std::shared_ptr<FireCell>>> cells_;
    std::unordered_set<Point> ticking_cells_;
    std::unordered_set<Point> burning_cells_;
    std::vector<Point> changed_cells_;

    // Random decives and Generators for the Cells
    std::random_device rd_;
    std::mt19937 gen_;

    std::vector<VirtualParticle> virtual_particles_;
    std::vector<RadiationParticle> radiation_particles_;
    template <typename ParticleType>
    void UpdateVirtualParticles(std::vector<ParticleType> &particles, std::vector<std::vector<bool>> &visited_cells);
    bool IsPointInGrid(int i, int j) {
        return !(i < 0 || i >= cols_ || j < 0 || j >= rows_);
    }
};


#endif //ROSHAN_FIREMODEL_GRIDMAP_H
