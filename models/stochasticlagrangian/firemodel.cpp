//
// Created by nex on 08.06.23.
//

#include "firemodel.h"

FireModel* FireModel::instance_ = nullptr;

FireModel::FireModel(SDL_Renderer *renderer) {
    model_renderer_ = FireModelRenderer::GetInstance(renderer);
    wind_ = new Wind(parameters_);
    Initialize();
}

void FireModel::Initialize() {
    model_renderer_->getWidthHeight(width_, height_);
    int rows = std::ceil(static_cast<float>(width_) / cell_size_);
    int cols = std::ceil(static_cast<float>(height_) / cell_size_);
    gridmap_ = new GridMap(rows, cols, wind_, parameters_);
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

void FireModel::Config() {
//    if (ImGui::Button("Randomize Cells"))
//        //RandomizeCells();
//    if (ImGui::Button("Reset Cells"))
//        //Reset();
    ImGui::Text("Window Options");
    ImGui::SliderInt("Cell Size", &cell_size_, 2, 100);
    ImGui::ColorEdit3("Background Colour", (float*)&model_renderer_->background_color_); // Edit 3 floats representing a color
    ImGui::Text("Simulation Options");
    ImGui::SliderScalar("Simulation Speed (dt)", ImGuiDataType_Double, &parameters_.dt_, &parameters_.min_dt_, &parameters_.max_dt_, "%.8f", 1.0f);
    if (ImGui::Button("Init new Grid"))
        Initialize();

    if (ImGui::TreeNode("Model Parameters")) {

        ImGui::Text("Virtual Particles");
        ImGui::SliderScalar("Particle Lifetime (tau_mem)", ImGuiDataType_Double, &parameters_.virtualparticle_tau_mem_, &parameters_.tau_min, &parameters_.tau_max, "%.3f", 1.0f);
        ImGui::SliderScalar("Hotness (Y_st)", ImGuiDataType_Double, &parameters_.virtualparticle_y_st_, &parameters_.min_Y_st_, &parameters_.max_Y_st_, "%.3f", 1.0f);
        ImGui::SliderScalar("Ignition Threshold (Y_lim)", ImGuiDataType_Double, &parameters_.virtualparticle_y_lim_, &parameters_.min_Y_lim_, &parameters_.max_Y_lim_, "%.3f", 1.0f);
        ImGui::SliderScalar("Height of Emission (Lt)", ImGuiDataType_Double, &parameters_.Lt_, &parameters_.min_Lt_, &parameters_.max_Lt_, "%.3f", 1.0f);
        ImGui::SliderScalar("Scaling Factor (Fl)", ImGuiDataType_Double, &parameters_.virtualparticle_fl_, &parameters_.min_Fl_, &parameters_.max_Fl_, "%.3f", 1.0f);
        ImGui::SliderScalar("Constant (C0)", ImGuiDataType_Double, &parameters_.virtualparticle_c0_, &parameters_.min_C0_, &parameters_.max_C0_, "%.3f", 1.0f);

        ImGui::Text("Radiation Particles");
        ImGui::SliderScalar("Radiation Hotness (Y_st)", ImGuiDataType_Double, &parameters_.radiationparticle_y_st_, &parameters_.min_Y_st_, &parameters_.max_Y_st_, "%.3f", 1.0f);
        ImGui::SliderScalar("Radiation Ignition Threshold (Y_lim)", ImGuiDataType_Double, &parameters_.radiationparticle_y_lim_, &parameters_.min_Y_lim_, &parameters_.max_Y_lim_, "%.3f", 1.0f);
        ImGui::SliderScalar("Height of Emission (Lr)", ImGuiDataType_Double, &parameters_.radiationparticle_Lr_, &parameters_.min_Lr_, &parameters_.max_Lr_, "%.3f", 1.0f);

        ImGui::Text("Cell (Terrain)");
        ImGui::SliderScalar("Ignition Threshold", ImGuiDataType_Double, &parameters_.cell_ignition_threshold_, &parameters_.min_ignition_threshold_, &parameters_.max_ignition_threshold_, "%.3f", 1.0f);
        ImGui::SliderScalar("Burning Duration", ImGuiDataType_Double, &parameters_.cell_burning_duration_, &parameters_.min_burning_duration_, &parameters_.max_burning_duration_, "%.3f", 1.0f);


        ImGui::Text("Wind");
        double min_angle_degree = 0;
        double max_angle_degree = 360;
        bool update_wind = false;
        if(ImGui::SliderScalar("Wind Speed", ImGuiDataType_Double, &parameters_.wind_uw_, &parameters_.min_Uw_, &parameters_.max_Uw_, "%.3f", 1.0f)) update_wind = true;
        if(ImGui::SliderScalar("A", ImGuiDataType_Double, &parameters_.wind_a_, &parameters_.min_A_, &parameters_.max_A_, "%.3f", 1.0f)) update_wind = true;
        if(ImGui::SliderScalar("Wind Angle", ImGuiDataType_Double, &parameters_.wind_angle_, &min_angle_degree, &max_angle_degree, "%.1f", 1.0f)) update_wind = true;
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
    model_renderer_->Render(gridmap_, cell_size_);
}

void FireModel::HandleEvents(SDL_Event event, ImGuiIO *io) {
    if (event.type == SDL_MOUSEBUTTONDOWN && !io->WantCaptureMouse) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        int _x = x / cell_size_;
        int _y = y / cell_size_;
        if ( _x < gridmap_->GetRows() && _y < gridmap_->GetCols())
            gridmap_->IgniteCell(_x, _y);
    }
}

void FireModel::SetWidthHeight(int width, int height) {
    width_ = width;
    height_ = height;
}
