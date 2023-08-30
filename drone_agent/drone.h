//
// Created by nex on 13.07.23.
//

#ifndef ROSHAN_DRONE_H
#define ROSHAN_DRONE_H

#include <utility>
#include <SDL.h>
#include <deque>
#include <memory>
#include "rendering/DroneRenderer.h"
#include "../models/stochasticlagrangian/model_parameters.h"
#include "../models/stochasticlagrangian/firemodel_gridmap.h"
#include "drone_state.h"

// TODO Remove circular dependency
class GridMap;

class DroneAgent {
public:
    explicit DroneAgent(std::shared_ptr<SDL_Renderer> renderer, FireModelParameters &parameters, int id);
    ~DroneAgent() = default;
    std::deque<DroneState> GetStates() { return drone_states_; }
    void Update(double speed_x, double speed_y, std::vector<std::vector<int>> terrain, std::vector<std::vector<int>> fire_status);
    void Move(double speed_x, double speed_y);
    bool DispenseWater(int water_dispense, GridMap &grid_map);
    std::pair<int, int> GetGridPosition();
    std::pair<double, double> GetGridPositionDouble();
    std::pair<double, double> GetRealPosition();
    DroneState GetLastState() { return drone_states_[0]; }
    int GetId() const { return id_; }
    int GetViewRange() const { return view_range_; }
    void Render(std::pair<int, int> position, int size);
    void Initialize(std::vector<std::vector<int>> terrain, std::vector<std::vector<int>> fire_status);
private:
    void UpdateStates(double speed_x, double speed_y, std::vector<std::vector<int>> terrain, std::vector<std::vector<int>> fire_status);
    int id_;
    FireModelParameters &parameters_;
    std::deque<DroneState> drone_states_;
    std::pair<double, double> position_; // x, y in (m)
    int view_range_;
    std::pair<double, double> velocity_; // angular & linear
    DroneRenderer renderer_;

};


#endif //ROSHAN_DRONE_H
