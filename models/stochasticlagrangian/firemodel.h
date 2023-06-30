//
// Created by nex on 08.06.23.
//

#ifndef ROSHAN_FIREMODEL_H
#define ROSHAN_FIREMODEL_H

#include <cstdlib>
#include <iostream>
#include <set>
#include <map>
#include "model_interface.h"
#include "firemodel_gridmap.h"
#include "rendering/firemodel_renderer.h"
#include "model_parameters.h"
#include "wind.h"
#include "CORINE/dataset_handler.h"
#include "externals/ImGuiFileDialog/ImGuiFileDialog.h"


class FireModel : public IModel{
public:
    //only one instance of this class can be created
    static FireModel* GetInstance(SDL_Renderer* renderer) {
        return instance_ = (instance_ != nullptr) ? instance_ : new FireModel(renderer);
    }

    void Initialize() override;
    void Update() override;
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
    ~FireModel() override {delete gridmap_; delete model_renderer_; delete wind_; delete dataset_handler_;}

    void RandomizeCells();

    GridMap* gridmap_;
    FireModelRenderer* model_renderer_;
    Wind* wind_;
    FireModelParameters parameters_;
    static FireModel* instance_;
    double running_time_;

    void OpenBrowser(std::string url);
    DatasetHandler* dataset_handler_;
    void ResetGridMap(std::vector<std::vector<int>>* rasterData = nullptr);

    //For the Popup of Cells
    bool show_popup_ = false;
    std::set<std::pair<int, int>> popups_;
    std::map<std::pair<int, int>, bool> popup_has_been_opened_;

    //current data
    std::vector<std::vector<int>> current_raster_data_;

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
    bool map_is_uniform_;

    void ShowParameterConfig();

    bool ImGuiOnStartup();

    void SetUniformRasterData();
};


#endif //ROSHAN_FIREMODEL_H
