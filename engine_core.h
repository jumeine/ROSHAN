//
// Created by nex on 06.06.23.
//

#ifndef ROSHAN_ENGINE_CORE_H
#define ROSHAN_ENGINE_CORE_H

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <chrono>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <stdio.h>
#include <SDL.h>
#include <iostream>
#include "model_interface.h"
#include "models/gameoflife/gameoflife_infinite.h"
#include "models/gameoflife_simple/gameoflife_fixed.h"
#include "models/stochasticlagrangian/firemodel.h"
#include "CORINE/dataset_handler.h"
#include "agent/memory.h"
#include "agent/drone_agent.h"
#include "agent.h"
#include "action.h"

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
    void ImGuiSimulationControls(bool &update_simulation, bool &render_simulation, int &delay);

private:
    EngineCore(){}
    ~EngineCore(){}

    bool is_running_;

    SDL_Window* window_;
    SDL_Renderer* renderer_;
    SDL_WindowFlags window_flags_;
    SDL_Texture* back_buffer_;

    ImGuiIO* io_;

    // Our model
    IModel* model_;

    // Our Simulation State
    bool update_simulation_ = false;
    bool render_simulation_ = true;
    int delay_ = 0;

    // For Init of the Window
    int width_ = 1600;
    int height_ = 900;

    // For the Node.js server
    void StartServer();
    void StopServer();

    static EngineCore* instance_;

    // ImGui Stuff
    bool ImGuiModelSelection();

    // AI Stuff
    Memory agent_memory_;
    std::shared_ptr<Agent> agent_;
};


#endif //ROSHAN_ENGINE_CORE_H
