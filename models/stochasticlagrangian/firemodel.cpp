//
// Created by nex on 08.06.23.
//

#include "firemodel.h"

FireModel* FireModel::instance_ = nullptr;

void FireModel::Initialize() {
    model_renderer_->getWidthHeight(width_, height_);
    int rows = std::ceil(static_cast<float>(width_) / cell_size_);
    int cols = std::ceil(static_cast<float>(height_) / cell_size_);
    wind_ = new Wind(0.3, 0.7);
    gridmap_ = new GridMap(rows, cols, wind_);
}

void FireModel::Update() {
    int Lt = 10; // in meter
    double dt = 1; // in seconds
    gridmap_->UpdateState(Lt, dt);
    gridmap_->UpdateCells(dt);
    gridmap_->HandleInteractions(dt);
}

void FireModel::Reset() {

}

void FireModel::Config() {
//    if (ImGui::Button("Randomize Cells"))
//        //RandomizeCells();
//    if (ImGui::Button("Reset Cells"))
//        //Reset();
    if (ImGui::Button("Init new Grid"))
        Initialize();

    ImGui::Text("Particle Parameters");
    // Go through virtual particles and get taumem
    double taumin = 0.1;
    double taumax = 50;
    if(ImGui::SliderScalar("Particle Lifetime", ImGuiDataType_Double, &tau_mem_, &taumin, &taumax, "%.3f", 1.0f))
        gridmap_->SetTauMem(tau_mem_);


    ImGui::Text("Wind Parameters");
    double Uw_i = wind_->GetWindSpeed();
    double A = wind_->GetA();
    ImGui::SliderScalar("Wind Speed", ImGuiDataType_Double, &Uw_i, &wind_->min_Uw_i_, &wind_->max_Uw_i_, "%.3f", 1.0f);
    ImGui::SliderScalar("A", ImGuiDataType_Double, &A, &wind_->min_A_, &wind_->max_A_, "%.3f", 1.0f);
    wind_->UpdateWind(Uw_i, A);

    ImGui::Text("Window Parameters");
    ImGui::SliderInt("Cell Size", &cell_size_, 2, 100);
    ImGui::ColorEdit3("Background Colour", (float*)&model_renderer_->background_color_); // Edit 3 floats representing a color

    ImGui::Text("Number of virtual particles: %d", gridmap_->GetNumParticles());
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
            gridmap_->IgniteCell(_x, _y, dt_);
    }
}

FireModel::FireModel(SDL_Renderer *renderer) {
    model_renderer_ = FireModelRenderer::GetInstance(renderer, gridmap_);
    Initialize();
}

void FireModel::SetWidthHeight(int width, int height) {
    width_ = width;
    height_ = height;
}
