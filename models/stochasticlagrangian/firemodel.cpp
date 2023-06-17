//
// Created by nex on 08.06.23.
//

#include "firemodel.h"

FireModel* FireModel::instance_ = nullptr;

FireModel::FireModel(SDL_Renderer *renderer) {
    model_renderer_ = FireModelRenderer::GetInstance(renderer, parameters_);
    wind_ = new Wind(parameters_);
    Initialize();
}

void FireModel::Initialize() {
    gridmap_ = new GridMap(wind_, parameters_);
    model_renderer_->SetGridMap(gridmap_);
    running_time_ = 0;
}

void FireModel::Update() {
    running_time_ += parameters_.GetDt();
    gridmap_->UpdateParticles();
    gridmap_->UpdateCells();
}

void FireModel::Reset() {

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
        ImGui::SetNextWindowSize(ImVec2(200, 100));

        if (ImGui::Begin(popupId, nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar)) {
            gridmap_->ShowCellInfo(it->first, it->second);
            if (ImGui::Button("Close")) {
                popup_has_been_opened_.erase(*it);  // Removes the popup from the map.
                it = popups_.erase(it);  // Removes the popup from the set and gets the next iterator.
                ImGui::End();  // End the window before going to the next iteration.
                continue;  // Skip the rest of the loop for this iteration.
            }
        }
        ImGui::End();  // End the window before going to the next iteration.
        ++it;  // Go to the next popup in the set.
    }
}

void FireModel::Config() {

    ImGui::Text("Window Options");
//    ImGui::SliderInt("Cell Size", &cell_size_, 2, 100);
    ImGui::SeparatorText("Simulation Options");
    ImGui::Text("Simulation Speed");
    ImGui::SliderScalar("dt", ImGuiDataType_Double, &parameters_.dt_, &parameters_.min_dt_, &parameters_.max_dt_, "%.8f", 1.0f);

    if (ImGui::TreeNodeEx("Model Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SeparatorText("Virtual Particles");
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

        ImGui::SeparatorText("Radiation Particles");
        ImGui::Text("Radiation Hotness");
        ImGui::SliderScalar("Y_st_r", ImGuiDataType_Double, &parameters_.radiationparticle_y_st_, &parameters_.min_Y_st_, &parameters_.max_Y_st_, "%.3f", 1.0f);
        ImGui::Text("Radiation Ignition Threshold");
        ImGui::SliderScalar("Y_lim_r", ImGuiDataType_Double, &parameters_.radiationparticle_y_lim_, &parameters_.min_Y_lim_, &parameters_.max_Y_lim_, "%.3f", 1.0f);
        ImGui::Text("Height of Emission");
        ImGui::SliderScalar("Lr", ImGuiDataType_Double, &parameters_.radiationparticle_Lr_, &parameters_.min_Lr_, &parameters_.max_Lr_, "%.3f", 1.0f);

        ImGui::SeparatorText("Cell (Terrain)");
        if (ImGui::Button("init new grid"))
            Initialize();
        ImGui::Text("Cell Size [init new grid afterwards]");
        ImGui::SliderScalar("##Cell Size", ImGuiDataType_Double, &parameters_.cell_size_, &parameters_.min_cell_size_, &parameters_.max_cell_size_, "%.3f", 1.0f);
        ImGui::Text("Cell Ignition Threshold");
        ImGui::SliderScalar("##Cell Ignition Threshold", ImGuiDataType_Double, &parameters_.cell_ignition_threshold_, &parameters_.min_ignition_threshold_, &parameters_.max_ignition_threshold_, "%.3f", 1.0f);
        ImGui::Text("Cell Burning Duration");
        ImGui::SliderScalar("##Cell Burning Duration", ImGuiDataType_Double, &parameters_.cell_burning_duration_, &parameters_.min_burning_duration_, &parameters_.max_burning_duration_, "%.3f", 1.0f);

        ImGui::SeparatorText("Wind");
        double min_angle_degree = 0;
        double max_angle_degree = 360;
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
    }

    if (ImGui::TreeNode("Analysis")) {
        ImGui::Text("Number of virtual particles: %d", gridmap_->GetNumParticles());
        ImGui::Text("Number of cells: %d", gridmap_->GetNumCells());
        ImGui::Text("Running Time in seconds: %.3f", running_time_);
        ImGui::TreePop();
    }
}

void FireModel::Render() {
    model_renderer_->Render(gridmap_);
}

void FireModel::HandleEvents(SDL_Event event, ImGuiIO *io) {
    if (event.type == SDL_MOUSEBUTTONDOWN && !io->WantCaptureMouse && event.button.button == SDL_BUTTON_LEFT) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        std::pair<int, int> gridPos = model_renderer_->ScreenToGridPosition(x, y);

        if (gridPos.first >= 0 && gridPos.first < gridmap_->GetRows() && gridPos.second >= 0 && gridPos.second < gridmap_->GetCols()) {
            if(gridmap_->GetCellState(gridPos.first, gridPos.second) == CellState::UNBURNED)
                gridmap_->IgniteCell(gridPos.first, gridPos.second);
            else if(gridmap_->GetCellState(gridPos.first, gridPos.second) == CellState::BURNING)
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
        if (cell_pos.first >= 0 && cell_pos.first < gridmap_->GetRows() && cell_pos.second >= 0 && cell_pos.second < gridmap_->GetCols()) {
            popups_.insert(cell_pos);
            popup_has_been_opened_.insert({cell_pos, false});
        }
    }
}

void FireModel::SetWidthHeight(int width, int height) {
    // Outdated
}
