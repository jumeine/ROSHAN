//
// Created by nex on 08.06.23.
//

#ifndef ROSHAN_FIREMODEL_RENDERER_H
#define ROSHAN_FIREMODEL_RENDERER_H

#include <SDL.h>
#include "firemodel_camera.h"
#include "firemodel_pixelbuffer.h"
#include "../firemodel_gridmap.h"
#include "models/stochasticlagrangian/particles/radiation_particle.h"
#include "models/stochasticlagrangian/particles/virtual_particle.h"
#include "../model_parameters.h"
#include "imgui.h"
#include "../utils.h"
#include <chrono>
#include <SDL_image.h>
#include <memory>
#include "agent/drone.h"

#include "../cell_classes/cell_generic_burned.cpp"
#include "../cell_classes/cell_generic_unburned.cpp"
#include "../cell_classes/cell_generic_burning.cpp"
#include "../cell_classes/cell_lichens_and_mosses.cpp"
#include "../cell_classes/cell_low_growing_woody_plants.cpp"
#include "../cell_classes/cell_non_and_sparsley_vegetated.cpp"
#include "../cell_classes/cell_outside_area.cpp"
#include "../cell_classes/cell_periodically_herbaceous.cpp"
#include "../cell_classes/cell_permanent_herbaceous.cpp"
#include "../cell_classes/cell_sealed.cpp"
#include "../cell_classes/cell_snow_and_ice.cpp"
#include "../cell_classes/cell_water.cpp"
#include "../cell_classes/cell_woody_breadleaved_deciduous_trees.cpp"
#include "../cell_classes/cell_woody_broadleaved_evergreen_trees.cpp"
#include "../cell_classes/cell_woody_needle_leaved_trees.cpp"

class FireModelRenderer {
public:
    //only one instance of this class is allowed
    static std::shared_ptr<FireModelRenderer> GetInstance(std::shared_ptr<SDL_Renderer> renderer, FireModelParameters& parameters) {
        if (instance_ == nullptr) {
            instance_ = std::shared_ptr<FireModelRenderer>(new FireModelRenderer(renderer, parameters));
        }
        return instance_;    }

    void Render(std::shared_ptr<std::vector<std::shared_ptr<DroneAgent>>> drones);
    void SetScreenResolution();
    void SetGridMap(std::shared_ptr<GridMap> gridmap) { gridmap_ = gridmap; SetFullRedraw(); }
    std::shared_ptr<SDL_Renderer> GetRenderer() { return renderer_; }

    // Converter Functions
    std::pair<int, int> ScreenToGridPosition(int x, int y);
    ImVec4 GetMappedColor(int cell_type);

    // Camera functions
    void CheckCamera();
    void ChangeCameraPosition(double x, double y) { camera_.Move(x, y); SetFullRedraw();}
    void ApplyZoom(double z) { camera_.Zoom(z); SetFullRedraw();}

    // Drawing Related
    void SetFullRedraw() { needs_full_redraw_ = true; }
    void ResizeEvent();
    void DrawArrow(double angle);

    ~FireModelRenderer();

private:
    FireModelRenderer(std::shared_ptr<SDL_Renderer> renderer, FireModelParameters& parameters);

    void DrawCells();
    void DrawCircle(int x, int y, int min_radius, double intensity);
    void DrawParticles();

    FireModelParameters& parameters_;
    FireModelCamera camera_;
    std::shared_ptr<SDL_Renderer> renderer_;
    SDL_Texture* texture_;
    PixelBuffer* pixel_buffer_;
    SDL_PixelFormat* pixel_format_;
    SDL_Texture* arrow_texture_;

    std::shared_ptr<GridMap> gridmap_;
    int width_;
    int height_;

    static std::shared_ptr<FireModelRenderer> instance_;

    bool needs_full_redraw_;
    void DrawAllCells(int grid_left, int grid_right, int grid_top, int grid_bottom);
    void DrawChangesCells();
    SDL_Rect DrawCell(int x, int y);

    void ResizePixelBuffer();
    void ResizeTexture();
    void DrawDrones(std::shared_ptr<std::vector<std::shared_ptr<DroneAgent>>> drones);
};


#endif //ROSHAN_FIREMODEL_RENDERER_H
