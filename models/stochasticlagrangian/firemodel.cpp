//
// Created by nex on 08.06.23.
//

#include "firemodel.h"

FireModel* FireModel::instance_ = nullptr;

FireModel::FireModel(SDL_Renderer *renderer) {
    dataset_handler_ = std::make_shared<DatasetHandler>("/home/nex/Downloads/CLMS_CLCplus_RASTER_2018_010m_eu_03035_V1_1/Data/CLMS_CLCplus_RASTER_2018_010m_eu_03035_V1_1.tif");
    drones_ = std::make_shared<std::vector<std::shared_ptr<DroneAgent>>>();
    model_renderer_ = FireModelRenderer::GetInstance(renderer, drones_, parameters_);
    wind_ = std::make_shared<Wind>(parameters_);
    gridmap_ = nullptr;
    running_time_ = 0;
}

void FireModel::ResetGridMap(std::vector<std::vector<int>>* rasterData) {
    gridmap_ = std::make_shared<GridMap>(wind_, parameters_, rasterData);
    model_renderer_->SetGridMap(gridmap_);
    // Init drones
    drones_->clear();
    for (int i = 0; i < parameters_.GetNumberOfDrones(); ++i) {
        drones_->push_back(std::make_shared<DroneAgent>(model_renderer_->GetRenderer()));
    }
    for(auto &drone : *drones_) {
        std::pair<std::vector<std::vector<int>>, std::vector<std::vector<int>>> drone_view = gridmap_->GetDroneView(drone);
        drone->Initialize(drone_view.first, drone_view.second);
    }
//    model_renderer_->CheckCamera();
    running_time_ = 0;
}

void FireModel::SetUniformRasterData() {
    current_raster_data_.clear();
    current_raster_data_ = std::vector<std::vector<int>>(parameters_.GetGridNx(), std::vector<int>(parameters_.GetGridNy(), GENERIC_UNBURNED));
    parameters_.map_is_uniform_ = true;
}

std::vector<std::deque<std::shared_ptr<State>>> FireModel::GetObservations() {
    std::vector<std::deque<std::shared_ptr<State>>> all_drone_states;
    if (gridmap_ != nullptr) {
        //Get observations
        for (auto &drone : *drones_) {
            std::deque<DroneState> drone_states = drone->GetStates();
            std::deque<std::shared_ptr<State>> shared_states;
            for (auto &state : drone_states) {
                shared_states.push_back(std::make_shared<DroneState>(state));
            }
            all_drone_states.push_back(shared_states);
        }

        return all_drone_states;
    }
    return {};
}

std::tuple<std::vector<std::deque<std::shared_ptr<State>>>, std::vector<double>, std::vector<bool>> FireModel::Step(std::vector<std::shared_ptr<Action>> actions) {
    std::vector<std::deque<std::shared_ptr<State>>> all_drone_states;
    std::vector<double> rewards;
    std::vector<bool> dones;
    if (gridmap_ != nullptr) {
        // Simulation time step update
        running_time_ += parameters_.GetDt();

        // Update the fire particles and the cell states
        gridmap_->UpdateParticles();
        gridmap_->UpdateCells();

        // Move the drones and get the next_observation
        for (int i = 0; i < (*drones_).size(); ++i) {
            double angular = std::dynamic_pointer_cast<DroneAction>(actions[i])->GetAngular();
            double linear = std::dynamic_pointer_cast<DroneAction>(actions[i])->GetLinear();
            drones_->at(i)->Move(angular, linear);
            std::pair<std::vector<std::vector<int>>, std::vector<std::vector<int>>> drone_view = gridmap_->GetDroneView(drones_->at(i));
            drones_->at(i)->Update(angular, linear, drone_view.first, drone_view.second);

            std::deque<DroneState> drone_states = drones_->at(i)->GetStates();
            std::deque<std::shared_ptr<State>> shared_states;
            for (auto &state : drone_states) {
                shared_states.push_back(std::make_shared<DroneState>(state));
            }
            all_drone_states.push_back(shared_states);
            rewards.push_back(0);
            dones.push_back(false);
        }

        return {all_drone_states, rewards, dones};

    }
    return {};
}

void FireModel::Render() {
    model_renderer_->Render();
    model_renderer_->DrawArrow(wind_->GetCurrentAngle() * 180 / M_PI + 45);
}

void FireModel::HandleEvents(SDL_Event event, ImGuiIO *io) {
    // SDL Events
    if (event.type == SDL_MOUSEBUTTONDOWN && !io->WantCaptureMouse && event.button.button == SDL_BUTTON_LEFT) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        std::pair<int, int> gridPos = model_renderer_->ScreenToGridPosition(x, y);
        x = gridPos.first;
        y = gridPos.second;
        if (x >= 0 && x < gridmap_->GetCols() && y >= 0 && y < gridmap_->GetRows()) {
            if(gridmap_->At(x, y).CanIgnite())
                gridmap_->IgniteCell(x, y);
            else if(gridmap_->GetCellState(x, y) == CellState::GENERIC_BURNING)
                gridmap_->ExtinguishCell(x, y);
        }
    } else if (event.type == SDL_MOUSEWHEEL && !io->WantCaptureMouse) {
        if (event.wheel.y > 0) // scroll up
        {
            model_renderer_->ApplyZoom(1.1);
        }
        else if (event.wheel.y < 0) // scroll down
        {
            model_renderer_->ApplyZoom(0.9);
        }
    } else if (event.type == SDL_MOUSEMOTION && !io->WantCaptureMouse) {
        int x, y;
        Uint32 mouseState = SDL_GetMouseState(&x, &y);
        if (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) // Middle mouse button is pressed
        {
            model_renderer_->ChangeCameraPosition(-event.motion.xrel, -event.motion.yrel);
        }
    } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_MIDDLE && !io->WantCaptureMouse) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        std::pair<int, int> cell_pos = model_renderer_->ScreenToGridPosition(x, y);
        if (cell_pos.first >= 0 && cell_pos.first < gridmap_->GetCols() && cell_pos.second >= 0 && cell_pos.second < gridmap_->GetRows()) {
            popups_.insert(cell_pos);
            popup_has_been_opened_.insert({cell_pos, false});
        }
    } else if (event.type == SDL_WINDOWEVENT) {
        if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            model_renderer_->ResizeEvent();
        }
    } else if (event.type == SDL_KEYDOWN && parameters_.GetNumberOfDrones() == 1) {
        std::pair<std::vector<std::vector<int>>, std::vector<std::vector<int>>> drone_view = gridmap_->GetDroneView(drones_->at(0));

        if (event.key.keysym.sym == SDLK_w) {
            drones_->at(0)->Update(0, parameters_.GetDroneLinearVelocity(0.1), drone_view.first, drone_view.second);
        }
        if (event.key.keysym.sym == SDLK_s) {
            drones_->at(0)->Update(0, -parameters_.GetDroneLinearVelocity(0.1), drone_view.first, drone_view.second);
        }
        if (event.key.keysym.sym == SDLK_a) {
            drones_->at(0)->Update(-parameters_.GetDroneAngularVelocity(0.5), 0, drone_view.first, drone_view.second);
        }
        if (event.key.keysym.sym == SDLK_d) {
            drones_->at(0)->Update(parameters_.GetDroneAngularVelocity(0.5), 0, drone_view.first, drone_view.second);
        }
        if (event.key.keysym.sym == SDLK_SPACE) {
            std::pair<double, double> position = drones_->at(0)->GetPosition();
            int x = static_cast<int>(position.first);
            int y = static_cast<int>(position.second);
            if(gridmap_->GetCellState(x, y) == CellState::GENERIC_BURNING)
                gridmap_->ExtinguishCell(x, y);
        }
    }
    // Browser Events
    // TODO: Eventloop only gets executed when Application is in Focus. Fix this.
    if (dataset_handler_ != nullptr) {
        if (dataset_handler_->NewDataPointExists() && browser_selection_flag_) {
            std::vector<std::vector<int>> rasterData;
            dataset_handler_->LoadRasterDataFromJSON(rasterData);
            current_raster_data_.clear();
            current_raster_data_ = rasterData;
            browser_selection_flag_ = false;
            parameters_.map_is_uniform_ = false;
            ResetGridMap(&current_raster_data_);
        }
    }
}

void FireModel::OpenBrowser(std::string url) {
    std::string command;

#if defined(_WIN32)
    command = std::string("start ");
#elif defined(__APPLE__)
    command = std::string("open ");
#elif defined(__linux__)
    command = std::string("xdg-open ");
#endif

    // system call
    if(system((command + url).c_str()) == -1) {
        std::cerr << "Error opening URL: " << url << std::endl;
    }
}

void FireModel::Initialize() {
    // Outdated code
}

void FireModel::SetWidthHeight(int width, int height) {
    // Outdated code
}

void FireModel::Reset() {
    // Outdated code
}

//** ImGui Stuff **//

void FireModel::ShowControls(std::function<void(bool &, bool &, int &)> controls, bool &update_simulation, bool &render_simulation, int &delay) {
    if (show_controls_)
        controls(update_simulation, render_simulation, delay);
}

void FireModel::ImGuiSimulationSpeed(){
    ImGui::Text("Simulation Speed");
    ImGui::SliderScalar("dt", ImGuiDataType_Double, &parameters_.dt_, &parameters_.min_dt_, &parameters_.max_dt_, "%.8f", 1.0f);
    ImGui::Spacing();
}

void FireModel::ImGuiModelMenu() {
    if (model_startup_) {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Open Browser")) {
                    std::string url = "http://localhost:3000/map.html";
                    OpenBrowser(url);
                }
                if (ImGui::MenuItem("Load Map from Disk")) {
                    // Open File Dialog
                    open_file_dialog_ = true;
                    // Load Map
                    load_map_from_disk_ = true;
                }
                if (ImGui::MenuItem("Load Map from Browser Selection"))
                    browser_selection_flag_ = true;
                if (ImGui::MenuItem("Load Uniform Map")) {
                    SetUniformRasterData();
                    ResetGridMap(&current_raster_data_);
                }
                if (ImGui::MenuItem("Save Map")) {
                    // Open File Dialog
                    open_file_dialog_ = true;
                    // Save Map
                    save_map_to_disk_ = true;
                }
                if (ImGui::MenuItem("Reset GridMap"))
                    ResetGridMap(&current_raster_data_);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Model Controls")) {
                ImGui::MenuItem("Emit Convective Particles", NULL, &parameters_.emit_convective_);
                ImGui::MenuItem("Emit Radiation Particles", NULL, &parameters_.emit_radiation_);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View")) {
                ImGui::MenuItem("Show Controls", NULL, &show_controls_);
                ImGui::MenuItem("Show Analysis", NULL, &show_model_analysis_);
                ImGui::MenuItem("Show Parameter Config", NULL, &show_model_parameter_config_);
                if(ImGui::MenuItem("Render Grid", NULL, &parameters_.render_grid_))
                    model_renderer_->SetFullRedraw();
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Help")) {
                ImGui::MenuItem("ImGui Help", NULL, &show_demo_window_);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }
}

void FireModel::Config() {
    if(show_demo_window_)
        ImGui::ShowDemoWindow(&show_demo_window_);

    if (!ImGuiOnStartup()) {

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 7));

        if(show_model_parameter_config_)
            ShowParameterConfig();

        if (show_model_analysis_) {
            ImGui::Begin("Analysis");
            ImGui::Spacing();
            ImGui::Text("Number of particles: %d", gridmap_->GetNumParticles());
            ImGui::Text("Number of cells: %d", gridmap_->GetNumCells());
            ImGui::Text("Running Time: %s", formatTime(running_time_).c_str());
            ImGui::Text("Höhe: %.2fkm | Breite: %.2fkm", gridmap_->GetRows() * parameters_.GetCellSize() / 1000,
                                                              gridmap_->GetCols() * parameters_.GetCellSize() / 1000);
            ImGui::End();
        }

        if (open_file_dialog_) {
            std::string filePathName;
            // open Dialog Simple
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".tif", "../maps/");

            // display
            if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
                // action if OK
                if (ImGuiFileDialog::Instance()->IsOk()) {
                    filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                    std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                    if (load_map_from_disk_){
                        dataset_handler_->LoadMap(filePathName);
                        std::vector<std::vector<int>> rasterData;
                        dataset_handler_->LoadMapDataset(rasterData);
                        current_raster_data_.clear();
                        current_raster_data_ = rasterData;
                        parameters_.map_is_uniform_ = false;
                        load_map_from_disk_ = false;
                        init_gridmap_ = true;
                    }
                    else if (save_map_to_disk_) {
                        dataset_handler_->SaveRaster(filePathName);
                        save_map_to_disk_ = false;
                    }
                }
                // close
                ImGuiFileDialog::Instance()->Close();
                open_file_dialog_ = false;
            }
        }
        ImGui::PopStyleVar();
    }
}

bool FireModel::ImGuiOnStartup() {
    if (!model_startup_) {
        int width, height;
        SDL_GetRendererOutputSize(model_renderer_->GetRenderer(), &width, &height);
        ImVec2 window_size = ImVec2(400, 100);
        ImGui::SetNextWindowSize(window_size);
        ImVec2 appWindowPos = ImVec2((width - window_size.x) * 0.5f, (height - window_size.y) * 0.5f);
        ImGui::SetNextWindowPos(appWindowPos);

        ImGui::Begin("Startup Mode Selection", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
        ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.45f, 0.6f, 0.85f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.45f, 0.7f, 0.95f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.45f, 0.5f, 0.75f, 1.0f));

        bool still_no_init = true;
        if (ImGui::Button("Uniform Vegetation", ImVec2(-1, 0))) {
            SetUniformRasterData();
            ResetGridMap(&current_raster_data_);
            still_no_init = false;
            model_startup_ = true;
            show_controls_ = true;
            show_model_parameter_config_ = true;
        }
        ImGui::Separator();
        if (ImGui::Button("Load from File", ImVec2(-1, 0))) {
            model_startup_ = true;
            show_controls_ = true;
            show_model_parameter_config_ = true;
            open_file_dialog_ = true;
            load_map_from_disk_ = true;
        }
        ImGui::PopStyleColor(3);
        ImGui::End();
        return still_no_init;
    } else {
        return false;
    }
}

void FireModel::ShowParameterConfig() {
    ImGui::Begin("Parameter Config");
    if (parameters_.emit_convective_) {
        ImGui::SeparatorText("Virtual Particles");
        if (ImGui::TreeNodeEx("##Virtual Particles",
                              ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth)) {
            ImGui::Text("Particle Lifetime");
            ImGui::SliderScalar("tau_mem", ImGuiDataType_Double, &parameters_.virtualparticle_tau_mem_,
                                &parameters_.tau_min, &parameters_.tau_max, "%.3f", 1.0f);
            ImGui::Text("Hotness");
            ImGui::SliderScalar("Y_st", ImGuiDataType_Double, &parameters_.virtualparticle_y_st_,
                                &parameters_.min_Y_st_, &parameters_.max_Y_st_, "%.3f", 1.0f);
            ImGui::Text("Ignition Threshold");
            ImGui::SliderScalar("Y_lim", ImGuiDataType_Double, &parameters_.virtualparticle_y_lim_,
                                &parameters_.min_Y_lim_, &parameters_.max_Y_lim_, "%.3f", 1.0f);
            ImGui::Text("Height of Emission");
            ImGui::SliderScalar("Lt", ImGuiDataType_Double, &parameters_.Lt_, &parameters_.min_Lt_,
                                &parameters_.max_Lt_, "%.3f", 1.0f);
            ImGui::Text("Scaling Factor");
            ImGui::SliderScalar("Fl", ImGuiDataType_Double, &parameters_.virtualparticle_fl_, &parameters_.min_Fl_,
                                &parameters_.max_Fl_, "%.3f", 1.0f);
            ImGui::Text("Constant");
            ImGui::SliderScalar("C0", ImGuiDataType_Double, &parameters_.virtualparticle_c0_, &parameters_.min_C0_,
                                &parameters_.max_C0_, "%.3f", 1.0f);
            ImGui::TreePop();
            ImGui::Spacing();
        }
    }

    if (parameters_.emit_radiation_) {
        ImGui::SeparatorText("Radiation Particles");
        if(ImGui::TreeNodeEx("##Radiation Particles", ImGuiTreeNodeFlags_SpanAvailWidth)){
            ImGui::Text("Radiation Hotness");
            ImGui::SliderScalar("Y_st_r", ImGuiDataType_Double, &parameters_.radiationparticle_y_st_, &parameters_.min_Y_st_, &parameters_.max_Y_st_, "%.3f", 1.0f);
            ImGui::Text("Radiation Ignition Threshold");
            ImGui::SliderScalar("Y_lim_r", ImGuiDataType_Double, &parameters_.radiationparticle_y_lim_, &parameters_.min_Y_lim_, &parameters_.max_Y_lim_, "%.3f", 1.0f);
//            ImGui::Text("Radiation Length");
//            ImGui::SliderScalar("Lr", ImGuiDataType_Double, &parameters_.radiationparticle_Lr_, &parameters_.min_Lr_, &parameters_.max_Lr_, "%.3f", 1.0f);
            ImGui::TreePop();
            ImGui::Spacing();
        }
    }

    if (parameters_.map_is_uniform_) {
        ImGui::SeparatorText("Cell (Terrain)");
        if(ImGui::TreeNodeEx("##CellTerrain", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth)) {
            ImGui::Spacing();
            ImGui::Text("Cell Size");
            //Colored Text in grey on the same line
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "(?)");
            if(ImGui::IsItemHovered())
                ImGui::SetTooltip("always press [Reset GridMap] manually after changing these values");
            ImGui::SliderScalar("##Cell Size", ImGuiDataType_Double, &parameters_.cell_size_, &parameters_.min_cell_size_, &parameters_.max_cell_size_, "%.3f", 1.0f);
            ImGui::Text("Cell Ignition Threshold");
            ImGui::SliderScalar("##Cell Ignition Threshold", ImGuiDataType_Double, &parameters_.cell_ignition_threshold_, &parameters_.min_ignition_threshold_, &parameters_.max_ignition_threshold_, "%.3f", 1.0f);
            ImGui::Text("Cell Burning Duration");
            ImGui::SliderScalar("##Cell Burning Duration", ImGuiDataType_Double, &parameters_.cell_burning_duration_, &parameters_.min_burning_duration_, &parameters_.max_burning_duration_, "%.3f", 1.0f);
            ImGui::TreePop();
            ImGui::Spacing();
        }
    }

    ImGui::SeparatorText("Wind");
    if(ImGui::TreeNodeEx("##Wind", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth)) {
        double min_angle_degree = 0;
        double max_angle_degree = (2* M_PI);
        bool update_wind = false;
        ImGui::Text("Wind Speed");
        if(ImGui::SliderScalar("##Wind Speed", ImGuiDataType_Double, &parameters_.wind_uw_, &parameters_.min_Uw_, &parameters_.max_Uw_, "%.3f", 1.0f)) update_wind = true;
        ImGui::Text("A");
        if(ImGui::SliderScalar("##A", ImGuiDataType_Double, &parameters_.wind_a_, &parameters_.min_A_, &parameters_.max_A_, "%.3f", 1.0f)) update_wind = true;
        ImGui::Text("Wind Angle");
        if(ImGui::SliderScalar("##Wind Angle", ImGuiDataType_Double, &parameters_.wind_angle_, &min_angle_degree, &max_angle_degree, "%.1f", 1.0f)) update_wind = true;
        if (update_wind)
            wind_->UpdateWind();
        ImGui::TreePop();
        ImGui::Spacing();
    }
    ImGui::End();
}

void FireModel::ShowPopups() {
    // Start the Dear ImGui frame
    for (auto it = popups_.begin(); it != popups_.end();) {
        char popupId[20];
        snprintf(popupId, sizeof(popupId), "Cell %d %d", it->first, it->second);

        if (!popup_has_been_opened_[*it]){
            ImGui::SetNextWindowPos(ImGui::GetMousePos());
            popup_has_been_opened_[*it] = true;
        }

        if (ImGui::Begin(popupId, nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar)) {
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 1.0f, 1.0f), "Cell %d %d", it->first, it->second);
            ImGui::SameLine();
            float windowWidth = ImGui::GetWindowContentRegionWidth();
            float buttonWidth = ImGui::CalcTextSize("X").x + ImGui::GetStyle().FramePadding.x;
            ImGui::SetCursorPosX(windowWidth - buttonWidth);
            if (ImGui::Button("X")) {
                popup_has_been_opened_.erase(*it);  // Removes the popup from the map.
                it = popups_.erase(it);  // Removes the popup from the set and gets the next iterator.
                ImGui::End();
                continue;  // Skip the rest of the loop for this iteration.
            }
            gridmap_->ShowCellInfo(it->first, it->second);
        }
        ImGui::End();
        ++it;  // Go to the next popup in the set.
    }
    if(init_gridmap_) {
        init_gridmap_ = false;
        ResetGridMap(&current_raster_data_);
    }
}

FireModel::~FireModel() {
}