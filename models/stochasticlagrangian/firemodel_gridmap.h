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
#include "wind.h"

class GridMap {
public:
    GridMap(int rows, int cols, Wind* wind);
    ~GridMap();

    int GetRows();
    int GetCols();
    void IgniteCell(int i, int j, double dt);
    void UpdateState(double Lt, double dt);
    void UpdateParticles(double u_prime, double Lt, double Uw_i, double dt);
    void UpdateCells(double dt);
    void HandleInteractions(double dt);
    int GetNumParticles();
    void SetTauMem(double tau_mem);

    FireCell& At(int i, int j) {
        return *cells_[i][j];
    }

private:
    Wind* wind_;
    int rows_;
    int cols_;
    std::vector<std::vector<FireCell*>> cells_;
    std::unordered_set<Point> ticking_cells_;
    std::unordered_set<Point> burning_cells_;

    std::vector<VirtualParticle> virtual_particles_;
    std::vector<RadiationParticle> radiation_particles_;
};


#endif //ROSHAN_FIREMODEL_GRIDMAP_H
