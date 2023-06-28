//
// Created by nex on 07.06.23.
//

#ifndef ROSHAN_GAMEOFLIFE_INFINITE_H
#define ROSHAN_GAMEOFLIFE_INFINITE_H

#include "../../model_interface.h"
#include "gameoflife_types.h"
#include "gameoflife_infinite_renderer.h"
#include "imgui.h"
#include <SDL.h>
#include <random>
#include <chrono>


class GameOfLifeInfinite : public IModel{

public:
    //only one instance of this class can be created
    static GameOfLifeInfinite* GetInstance(SDL_Renderer* renderer) {
        return instance_ = (instance_ != nullptr) ? instance_ : new GameOfLifeInfinite(renderer);
    }

    void Initialize() override;
    void Update() override;
    void Reset() override;
    void Config() override;
    void Render() override;
    void SetWidthHeight(int width, int height) override;
    void HandleEvents(SDL_Event event, ImGuiIO* io) override;
    void ShowPopups() override;
    void ImGuiSimulationSpeed() override;
    void ImGuiModelMenu() override;
    void ShowControls(std::function<void(bool&, bool&, int&)> controls, bool &update_simulation, bool &render_simulation, int &delay) override;

private:
    GameOfLifeInfinite(SDL_Renderer* renderer);
    ~GameOfLifeInfinite(){}

    void RandomizeCells();

    CellStateGOF state_;
    Neighbors GetNeighbors(const Cell& cell) const;
    int CountLiveNeighbors(const Cell& cell) const;

    GameOfLifeInfiniteRenderer* model_renderer_;
    int width_ = 0;
    int height_ = 0;
    int cell_size_ = 10;


    //std::vector<std::vector<bool>> cellState;
    static GameOfLifeInfinite* instance_;

};


#endif //ROSHAN_GAMEOFLIFE_INFINITE_H
