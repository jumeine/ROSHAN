//
// Created by nex on 08.06.23.
//

#ifndef ROSHAN_FIREMODEL_H
#define ROSHAN_FIREMODEL_H

#include <cstdlib>
#include <iostream>
#include <set>
#include <map>
#include <chrono>
#include <thread>
#include "model_interface.h"
#include "firemodel_gridmap.h"
#include "rendering/firemodel_renderer.h"
#include "model_parameters.h"
#include "wind.h"
#include "CORINE/dataset_handler.h"
#include "externals/ImGuiFileDialog/ImGuiFileDialog.h"
#include "agent/drone.h"
#include "agent/drone_action.h"

class FireModel : public IModel{
public:
    //only one instance of this class can be created
    static FireModel* GetInstance(SDL_Renderer* renderer) {
        return instance_ = (instance_ != nullptr) ? instance_ : new FireModel(renderer);
    }

    void Initialize() override;
    std::tuple<std::vector<std::deque<std::shared_ptr<State>>>, std::vector<double>, std::vector<bool>> Step(std::vector<std::shared_ptr<Action>> actions) override;
    std::vector<std::deque<std::shared_ptr<State>>> GetObservations() override;
    void Reset() override;
    void Config() override;
    void Render() override;
    void SetWidthHeight(int width, int height) override;
    void HandleEvents(SDL_Event event, ImGuiIO* io) override;
    void ShowPopups() override;
    void ImGuiSimulationSpeed() override;
    void ImGuiModelMenu() override;
    void ShowControls(std::function<void(bool&, bool&, int&)> controls, bool &update_simulation, bool &render_simulation, int &delay) override;

private:
    explicit FireModel(SDL_Renderer* renderer);
    ~FireModel() override;

    void RandomizeCells();

    std::shared_ptr<GridMap> gridmap_;
    std::shared_ptr<FireModelRenderer> model_renderer_;
    std::shared_ptr<Wind> wind_;
    FireModelParameters parameters_;
    static FireModel* instance_;
    double running_time_;

    void OpenBrowser(std::string url);
    std::shared_ptr<DatasetHandler> dataset_handler_;
    void ResetGridMap(std::vector<std::vector<int>>* rasterData = nullptr);

    //For the Popup of Cells
    bool show_popup_ = false;
    std::set<std::pair<int, int>> popups_;
    std::map<std::pair<int, int>, bool> popup_has_been_opened_;

    //current data
    std::vector<std::vector<int>> current_raster_data_;
    //agent data
    std::shared_ptr<std::vector<std::shared_ptr<DroneAgent>>> drones_;
    std::vector<std::deque<DroneState>> observations_;

    //Flags
    bool browser_selection_flag_ = false;  // If set to true, will load a new GridMap from a file.
    bool show_demo_window_ = false;
    bool show_controls_ = false;
    bool show_model_analysis_ = false;
    bool show_model_parameter_config_ = false;
    bool model_startup_ = false;
    bool open_file_dialog_ = false;
    bool load_map_from_disk_ = false;
    bool save_map_to_disk_ = false;
    bool init_gridmap_ = false;

    void ShowParameterConfig();
    bool ImGuiOnStartup();
    void SetUniformRasterData();
};


#endif //ROSHAN_FIREMODEL_H
