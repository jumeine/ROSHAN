//
// Created by nex on 06.06.23.
//

#include "engine_core.h"

EngineCore* EngineCore::instance_ = nullptr;

bool EngineCore::Init(){
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        SDL_Log("Failed to initialize SDL: %s\n", SDL_GetError());
        return false;
    }

    // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    // Create window with SDL_Renderer graphics context
    window_flags_ = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL);
    window_ = SDL_CreateWindow("ROSHAN", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width_, height_, window_flags_);
    if (window_ == nullptr)
    {
        SDL_Log("Error creating SDL_Window: %s\n", SDL_GetError());
        return false;
    }
    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer_ == nullptr)
    {
        SDL_Log("Error creating SDL_Renderer: %s\n", SDL_GetError());
        return false;
    }
    //SDL_RendererInfo info;
    //SDL_GetRendererInfo(renderer, &info);
    //SDL_Log("Current SDL_Renderer: %s", info.name);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io_ = &ImGui::GetIO();
    //(void)io_;
    io_->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io_->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameBorderSize = 1.0f; // Set border size to 1.0f
    style.FrameRounding = 6.0f; // Set rounding to 5.0f
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    if (!ImGui_ImplSDL2_InitForSDLRenderer(window_, renderer_)){
        SDL_Log("Error initializing ImGui_ImplSDL2_InitForSDLRenderer: %s\n", SDL_GetError());
        return false;
    }
    if (!ImGui_ImplSDLRenderer2_Init(renderer_)){
        SDL_Log("Error initializing ImGui_ImplSDLRenderer2_Init: %s\n", SDL_GetError());
        return false;
    }

    //
    return is_running_ = true;
}

void EngineCore::Update() {
    if (update_simulation_) {
        model_->Update();
        SDL_Delay(delay_);
    }
}

void EngineCore::Render() {
    // Start the Dear ImGui frame
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // This is necessary because the window size does not update automatically when the window is maximized
    if (window_was_maximized_) {
        model_->Initialize();
        window_was_maximized_ = false;
        render_simulation_ = true;
    }

    if (show_demo_window_)
        ImGui::ShowDemoWindow(&show_demo_window_);

    // Choose model if model is not set
    if (model_ == nullptr) {
        ImGui::Begin("Choose Model");
        if (ImGui::Button("Game of Life (Infinite Grid)")) {
            model_ = GameOfLifeInfinite::GetInstance(renderer_);
        } else if (ImGui::Button("Game of Life (Fixed Grid)")) {
            model_ = GameOfLifeFixed::GetInstance(renderer_);
        } else if (ImGui::Button("Firemodel")) {
            SDL_MaximizeWindow(window_);
            window_was_maximized_ = true;
            render_simulation_ = false;
            model_ = FireModel::GetInstance(renderer_);
        }
        ImGui::End();
    } else {
        ImGui::Begin("Controls");
        SDL_GetWindowSize(window_, &width_, &height_);
        model_->SetWidthHeight(width_, height_);
        if (ImGui::TreeNode("Simulation Controls")) {
            ImVec4 updateColor = update_simulation_ ? ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] : ImGui::GetStyle().Colors[ImGuiCol_Button];
            if (ImGui::Button("Start/Stop Simulation")) {
                update_simulation_ = !update_simulation_;
            }
            ImGui::SameLine();
            ImGui::TextColored(updateColor, "%s", update_simulation_ ? "Simulation is running" : "Simulation is stopped");
            ImVec4 renderColor = render_simulation_ ? ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] : ImGui::GetStyle().Colors[ImGuiCol_Button];
            if (ImGui::Button("Render Simulation")) {
                render_simulation_ = !render_simulation_;
            }
            ImGui::SameLine();
            ImGui::TextColored(renderColor, "%s", render_simulation_ ? "Simulation is rendered" : "Simulation is not rendered");
            ImGui::Checkbox("Demo Window", &show_demo_window_);      // Edit bools storing our window open/close state
            ImGui::SliderInt("Delay (ms)", &delay_, 0, 500);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io_->Framerate, io_->Framerate);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Model Controls")) {
            model_->Config();
            ImGui::TreePop();
        }
        model_->ShowPopups();
        ImGui::End();
    }

    // Rendering
    ImGui::Render();
    SDL_RenderSetScale(renderer_, io_->DisplayFramebufferScale.x, io_->DisplayFramebufferScale.y);
    if (model_ != nullptr && render_simulation_) {
        model_->Render();
    }
    else {
        SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
        SDL_RenderClear(renderer_);
    }

    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(renderer_);
}

void EngineCore::HandleEvents() {
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT)
            is_running_ = false;
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window_))
            is_running_ = false;
        if (model_ != nullptr)
            model_->HandleEvents(event, io_);
    }
}

void EngineCore::Clean() {
    // Cleanup
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
}


