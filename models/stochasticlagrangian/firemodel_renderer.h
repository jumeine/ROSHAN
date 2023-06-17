//
// Created by nex on 08.06.23.
//

#ifndef ROSHAN_FIREMODEL_RENDERER_H
#define ROSHAN_FIREMODEL_RENDERER_H

#include <SDL.h>
#include "firemodel_gridmap.h"
#include "radiation_particle.h"
#include "virtual_particle.h"
#include "model_parameters.h"
#include "imgui.h"
#include "utils.h"

class FireModelRenderer {
public:
    //only one instance of this class is allowed
    static FireModelRenderer* GetInstance(SDL_Renderer* renderer, FireModelParameters& parameters) {
        return instance_ = (instance_ != nullptr) ? instance_ : new FireModelRenderer(renderer, parameters);
    }

    void Initialize();
    void Update();
    void Render(GridMap* gridmap);
    void GetScreenResolution(int& width, int& height);
    void SetGridMap(GridMap* gridmap) { gridmap_ = gridmap; }

    // Converter Functions
    std::pair<int, int> ScreenToGridPosition(int x, int y);
    // Cell size
    double GetCellSize();
    // Camera position
    double GetCamX() const { return static_cast<int>(std::round(camX_)); }
    double GetCamY() const { return static_cast<int>(std::round(camY_)); }
    void ChangeCameraPosition(double x, double y) { camX_ -= x * camera_speed_; camY_ -= y * camera_speed_;}
    // Zoom level
    void ApplyZoom(double z);
private:
    FireModelRenderer(SDL_Renderer* renderer, FireModelParameters& parameters);

    void DrawCells();
    void DrawGrid();
    void DrawCircle(int x, int y, int min_radius, double intensity);
    void DrawParticles();

    FireModelParameters& parameters_;
    SDL_Renderer* renderer_;
    GridMap* gridmap_;
    int width_;
    int height_;
    // Camera position
    const double camera_speed_ = 0.3;
    double camX_ = 0.0;
    double camY_ = 0.0;
    // Zoom level
    double zoom_ = 1.0;
    double show_grid_;

    static FireModelRenderer* instance_;
};


#endif //ROSHAN_FIREMODEL_RENDERER_H
