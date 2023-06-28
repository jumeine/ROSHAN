//
// Created by nex on 08.06.23.
//

#include "firemodel.h"

FireModel* FireModel::instance_ = nullptr;

FireModel::FireModel(SDL_Renderer *renderer) {
    dataset_handler_ = new DatasetHandler("/home/nex/Downloads/CLMS_CLCplus_RASTER_2018_010m_eu_03035_V1_1/Data/CLMS_CLCplus_RASTER_2018_010m_eu_03035_V1_1.tif");
    model_renderer_ = FireModelRenderer::GetInstance(renderer, parameters_);
    wind_ = new Wind(parameters_);
    gridmap_ = nullptr;
    running_time_ = 0;
}

void FireModel::Initialize() {
    gridmap_ = new GridMap(wind_, parameters_);
    model_renderer_->SetGridMap(gridmap_);
    running_time_ = 0;
}

void FireModel::ResetGridMap(std::vector<std::vector<int>>* rasterData) {
    delete gridmap_;
    gridmap_ = new GridMap(wind_, parameters_, rasterData);
    model_renderer_->SetGridMap(gridmap_);
    running_time_ = 0;
}


void FireModel::Update() {
    if (gridmap_ != nullptr) {
        running_time_ += parameters_.GetDt();
        gridmap_->UpdateParticles();
        gridmap_->UpdateCells();
    }
}

void FireModel::Reset() {

}

void FireModel::Render() {
    model_renderer_->Render();
}

void FireModel::HandleEvents(SDL_Event event, ImGuiIO *io) {
    // SDL Events
    if (event.type == SDL_MOUSEBUTTONDOWN && !io->WantCaptureMouse && event.button.button == SDL_BUTTON_LEFT) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        std::pair<int, int> gridPos = model_renderer_->ScreenToGridPosition(x, y);

        if (gridPos.first >= 0 && gridPos.first < gridmap_->GetCols() && gridPos.second >= 0 && gridPos.second < gridmap_->GetRows()) {
            if(gridmap_->GetCellState(gridPos.first, gridPos.second) != CellState::GENERIC_BURNING || gridmap_->GetCellState(gridPos.first, gridPos.second) != CellState::GENERIC_BURNED)
                gridmap_->IgniteCell(gridPos.first, gridPos.second);
            else if(gridmap_->GetCellState(gridPos.first, gridPos.second) == CellState::GENERIC_BURNING)
                gridmap_->ExtinguishCell(gridPos.first, gridPos.second);
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
    }
    // Browser Events
    // TODO: Eventloop only gets executed when Application is in Focus. Fix this.
    if (dataset_handler_ != nullptr) {
        if (dataset_handler_->NewDataPointExists() && browser_selection_flag_) {
            std::vector<std::vector<int>> rasterData;
            dataset_handler_->LoadRasterDataFromFile(rasterData);
            ResetGridMap(&rasterData);
            browser_selection_flag_ = false;
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

void FireModel::SetWidthHeight(int width, int height) {
    // Outdated
}

//** ImGui Controls **//

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
                if (ImGui::MenuItem("Open")) {
                    bool open = true;
//                std::string path = FileDialog::OpenFile();
//                if (!path.empty()) {
//                    dataset_handler_->OpenFile(path);
//                    ResetGridMap(dataset_handler_->GetRasterData());
//                }
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View")) {
                if (ImGui::MenuItem("Show Controls"))
                    show_controls_ = !show_controls_;
                if (ImGui::MenuItem("Show Analysis"))
                    show_model_analysis_ = !show_model_analysis_;
                if (ImGui::MenuItem("Show Parameter Config"))
                    show_model_parameter_config_ = !show_model_parameter_config_;
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Help")) {
                if (ImGui::MenuItem("ImGui Help")) {
                    show_demo_window_ = !show_demo_window_;
                }
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
            ImGui::Text("Number of virtual particles: %d", gridmap_->GetNumParticles());
            ImGui::Text("Number of cells: %d", gridmap_->GetNumCells());
            ImGui::Text("Running Time in seconds: %.3f", running_time_);
            ImGui::End();
        }

        ImGui::PopStyleVar();
    }
}

bool FireModel::ImGuiOnStartup() {
    if (!model_startup_) {
        int width, height;
        SDL_GetRendererOutputSize(model_renderer_->GetRenderer(), &width, &height);
        ImVec2 window_size = ImVec2(500, 150);
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
            Initialize();
            still_no_init = false;
            model_startup_ = true;
            show_controls_ = true;
            show_model_parameter_config_ = true;
        }
        ImGui::Separator();
        if (ImGui::Button("Load from Browser", ImVec2(-1, 0))) {

        }
        ImGui::Separator();
        if (ImGui::Button("Load from File", ImVec2(-1, 0))) {

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
    ImGui::SeparatorText("Virtual Particles");
    if(ImGui::TreeNodeEx("##Virtual Particles", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth)){
        ImGui::Text("Particle Lifetime");
        ImGui::SliderScalar("tau_mem", ImGuiDataType_Double, &parameters_.virtualparticle_tau_mem_, &parameters_.tau_min, &parameters_.tau_max, "%.3f", 1.0f);
        ImGui::Text("Hotness");
        ImGui::SliderScalar("Y_st", ImGuiDataType_Double, &parameters_.virtualparticle_y_st_, &parameters_.min_Y_st_, &parameters_.max_Y_st_, "%.3f", 1.0f);
        ImGui::Text("Ignition Threshold");
        ImGui::SliderScalar("Y_lim", ImGuiDataType_Double, &parameters_.virtualparticle_y_lim_, &parameters_.min_Y_lim_, &parameters_.max_Y_lim_, "%.3f", 1.0f);
        ImGui::Text("Height of Emission");
        ImGui::SliderScalar("Lt", ImGuiDataType_Double, &parameters_.Lt_, &parameters_.min_Lt_, &parameters_.max_Lt_, "%.3f", 1.0f);
        ImGui::Text("Scaling Factor");
        ImGui::SliderScalar("Fl", ImGuiDataType_Double, &parameters_.virtualparticle_fl_, &parameters_.min_Fl_, &parameters_.max_Fl_, "%.3f", 1.0f);
        ImGui::Text("Constant");
        ImGui::SliderScalar("C0", ImGuiDataType_Double, &parameters_.virtualparticle_c0_, &parameters_.min_C0_, &parameters_.max_C0_, "%.3f", 1.0f);
        ImGui::TreePop();
        ImGui::Spacing();
    }

    ImGui::SeparatorText("Radiation Particles");
    if(ImGui::TreeNodeEx("##Radiation Particles", ImGuiTreeNodeFlags_SpanAvailWidth)){
        ImGui::Text("Radiation Hotness");
        ImGui::SliderScalar("Y_st_r", ImGuiDataType_Double, &parameters_.radiationparticle_y_st_, &parameters_.min_Y_st_, &parameters_.max_Y_st_, "%.3f", 1.0f);
        ImGui::Text("Radiation Ignition Threshold");
        ImGui::SliderScalar("Y_lim_r", ImGuiDataType_Double, &parameters_.radiationparticle_y_lim_, &parameters_.min_Y_lim_, &parameters_.max_Y_lim_, "%.3f", 1.0f);
        ImGui::Text("Height of Emission");
        ImGui::SliderScalar("Lr", ImGuiDataType_Double, &parameters_.radiationparticle_Lr_, &parameters_.min_Lr_, &parameters_.max_Lr_, "%.3f", 1.0f);
        ImGui::TreePop();
        ImGui::Spacing();
    }

    ImGui::SeparatorText("Cell (Terrain)");
    if(ImGui::TreeNodeEx("##CellTerrain", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth)) {
        if (ImGui::Button("Open Browser")) {
            std::string url = "http://localhost:3000/map.html";
            OpenBrowser(url);
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset GridMap"))
            ResetGridMap();
        ImGui::SameLine();
        if (ImGui::Button("Load GridMap"))
            browser_selection_flag_ = true;
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
}