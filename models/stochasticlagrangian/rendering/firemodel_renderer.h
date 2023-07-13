//
// Created by nex on 08.06.23.
//

#ifndef ROSHAN_FIREMODEL_RENDERER_H
#define ROSHAN_FIREMODEL_RENDERER_H

#include <SDL.h>
#include "firemodel_camera.h"
#include "firemodel_pixelbuffer.h"
#include "../firemodel_gridmap.h"
#include "../radiation_particle.h"
#include "../virtual_particle.h"
#include "../model_parameters.h"
#include "imgui.h"
#include "../utils.h"
#include <chrono>
#include <SDL_image.h>
#include <memory>
#include "agent/drone.h"

class FireModelRenderer {
public:
    //only one instance of this class is allowed
    static std::shared_ptr<FireModelRenderer> GetInstance(SDL_Renderer* renderer,std::shared_ptr<std::vector<std::shared_ptr<DroneAgent>>> drones, FireModelParameters& parameters) {
        if (instance_ == nullptr) {
            instance_ = std::shared_ptr<FireModelRenderer>(new FireModelRenderer(renderer, drones, parameters));
        }
        return instance_;    }

    void Render();
    void SetScreenResolution();
    void SetGridMap(std::shared_ptr<GridMap> gridmap) { gridmap_ = gridmap; SetFullRedraw(); }
    SDL_Renderer* GetRenderer() { return renderer_; }

    // Converter Functions
    std::pair<int, int> ScreenToGridPosition(int x, int y);

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
    FireModelRenderer(SDL_Renderer* renderer, std::shared_ptr<std::vector<std::shared_ptr<DroneAgent>>> drones, FireModelParameters& parameters);

    void DrawCells();
    void DrawCircle(int x, int y, int min_radius, double intensity);
    void DrawParticles();

    FireModelParameters& parameters_;
    FireModelCamera camera_;
    SDL_Renderer* renderer_;
    SDL_Texture* texture_;
    PixelBuffer* pixel_buffer_;
    SDL_PixelFormat* pixel_format_;
    SDL_Texture* arrow_texture_;
    std::shared_ptr<std::vector<std::shared_ptr<DroneAgent>>> drones_;

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
    void DrawDrone();
};


#endif //ROSHAN_FIREMODEL_RENDERER_H
