//
// Created by nex on 06.06.23.
//

#ifndef ROSHAN_ENGINE_CORE_H
#define ROSHAN_ENGINE_CORE_H

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <stdio.h>
#include <SDL.h>
#include "utils.h"
#include <iostream>
#include "model_interface.h"
#include "models/gameoflife/gameoflife_infinite.h"
#include "models/gameoflife_simple/gameoflife_fixed.h"
#include "models/stochasticlagrangian/firemodel.h"

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

class EngineCore {

public:
    static EngineCore* GetInstance() {
        return instance_ = (instance_ != nullptr) ? instance_ : new EngineCore();
    }

    bool Init();
    void Clean();

    void Update();
    void Render();
    void HandleEvents();

    inline bool IsRunning() { return is_running_; }
    inline SDL_Renderer* GetRenderer() { return renderer_; }

private:
    EngineCore(){}
    ~EngineCore(){}

    bool is_running_;

    SDL_Window* window_;
    SDL_Renderer* renderer_;
    SDL_WindowFlags window_flags_;

    ImGuiIO* io_;

    // Our model
    IModel* model_;

    // Our Simulation State
    bool show_demo_window_ = false;
    bool update_simulation_ = false;
    bool render_simulation_ = true;
    int delay_ = 0;

    // For Init of the Window
    bool window_was_maximized_ = false;
    int width_ = 1000;
    int height_ = 750;

    static EngineCore* instance_;
};


#endif //ROSHAN_ENGINE_CORE_H
