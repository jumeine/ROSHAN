//
// Created by nex on 08.06.23.
//

#ifndef ROSHAN_FIREMODEL_RENDERER_H
#define ROSHAN_FIREMODEL_RENDERER_H

#include <SDL.h>
#include "firemodel_camera.h"
#include "../firemodel_gridmap.h"
#include "../radiation_particle.h"
#include "../virtual_particle.h"
#include "../model_parameters.h"
#include "imgui.h"
#include "../utils.h"

class FireModelRenderer {
public:
    //only one instance of this class is allowed
    static FireModelRenderer* GetInstance(SDL_Renderer* renderer, FireModelParameters& parameters) {
        return instance_ = (instance_ != nullptr) ? instance_ : new FireModelRenderer(renderer, parameters);
    }

    void Render();
    void GetScreenResolution(int& width, int& height);
    void SetGridMap(GridMap* gridmap) { gridmap_ = gridmap; }
    SDL_Renderer* GetRenderer() { return renderer_; }

    // Converter Functions
    std::pair<int, int> ScreenToGridPosition(int x, int y);

    // Camera functions
    void ChangeCameraPosition(double x, double y) { camera_.Move(x, y);}
    void ApplyZoom(double z) { camera_.Zoom(z);}

    ~FireModelRenderer();

private:
    FireModelRenderer(SDL_Renderer* renderer, FireModelParameters& parameters);

    void DrawCells();
    void DrawGrid();
    void DrawCircle(int x, int y, int min_radius, double intensity);
    void DrawParticles();

    FireModelParameters& parameters_;
    FireModelCamera camera_;
    SDL_Renderer* renderer_;
    SDL_Texture* back_buffer_;
    SDL_Surface* drawing_surface_;
    SDL_Texture* texture_;
    Uint32* pixel_buffer_;
    GridMap* gridmap_ = nullptr;
    int width_;
    int height_;
    // Camera position
    const double camera_speed_ = 0.3;

    static FireModelRenderer* instance_;
};


#endif //ROSHAN_FIREMODEL_RENDERER_H
