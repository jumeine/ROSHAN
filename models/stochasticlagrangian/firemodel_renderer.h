//
// Created by nex on 08.06.23.
//

#ifndef ROSHAN_FIREMODEL_RENDERER_H
#define ROSHAN_FIREMODEL_RENDERER_H

#include <SDL.h>
#include "firemodel_gridmap.h"
#include "radiation_particle.h"
#include "virtual_particle.h"
#include "imgui.h"
#include "utils.h"

class FireModelRenderer {
public:
    //only one instance of this class is allowed
    static FireModelRenderer* GetInstance(SDL_Renderer* renderer) {
        return instance_ = (instance_ != nullptr) ? instance_ : new FireModelRenderer(renderer);
    }

    void Initialize();
    void Update();
    void Render(GridMap* gridmap, int cell_size);
    void getWidthHeight(int& width, int& height);
    void SetGridMap(GridMap* gridmap) { gridmap_ = gridmap; }
    ImVec4 background_color_ = ImVec4(85/255.0f, 155/255.0f, 75/255.0f, 1.00f);

private:
    FireModelRenderer(SDL_Renderer* renderer);
    ~FireModelRenderer(){}

    void DrawCells(GridMap* gridmap, int cell_size);
    void DrawGrid(GridMap* gridmap, int cell_size);
    void DrawCircle(int x, int y, int radius);
    void DrawParticles(int cell_size);

    SDL_Renderer* renderer_;
    GridMap* gridmap_;
    int width_;
    int height_;

    static FireModelRenderer* instance_;
};


#endif //ROSHAN_FIREMODEL_RENDERER_H
