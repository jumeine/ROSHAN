//
// Created by nex on 07.06.23.
//

#ifndef ROSHAN_GAMEOFLIFESIMPLE_H
#define ROSHAN_GAMEOFLIFESIMPLE_H

#include "../../model_interface.h"
#include "gameoflife_fixed_renderer.h"
#include "imgui.h"
#include <SDL.h>
#include <random>
#include <chrono>


class GameOfLifeFixed : public IModel{

public:
    //only one instance of this class can be created
    static GameOfLifeFixed* GetInstance(SDL_Renderer* renderer) {
        return instance_ = (instance_ != nullptr) ? instance_ : new GameOfLifeFixed(renderer);
    }

    void Initialize() override;
    void Update() override;
    void Reset() override;
    void Config() override;
    void Render() override;
    void SetWidthHeight(int width, int height) override;
    void HandleEvents(SDL_Event event, ImGuiIO* io) override;
    void ShowPopups() override;


private:
    GameOfLifeFixed(SDL_Renderer* renderer);
    ~GameOfLifeFixed(){}

    void RandomizeCells();

    GameOfLifeFixedRenderer* model_renderer_;
    int width_ = 0;
    int height_ = 0;
    int cell_size_ = 10;

    int rows_;
    int cols_;
    int current_state_;
    std::vector<std::vector<bool>> state_[2];


    //std::vector<std::vector<bool>> cellState;
    static GameOfLifeFixed* instance_;

};


#endif //ROSHAN_GAMEOFLIFE_INFINITE_H
