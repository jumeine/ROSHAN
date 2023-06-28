//
// Created by nex on 08.06.23.
//

#ifndef ROSHAN_GAMEOFLIFE_INFINITE_RENDERER_H
#define ROSHAN_GAMEOFLIFE_INFINITE_RENDERER_H

#include "imgui.h"
#include "gameoflife_types.h"
#include <vector>
#include <SDL.h>
#include <unordered_map>

class GameOfLifeInfiniteRenderer{

public:
    GameOfLifeInfiniteRenderer();

//only one instance of this class can be created
    static GameOfLifeInfiniteRenderer* GetInstance(SDL_Renderer* renderer) {
        return instance_ = (instance_ != nullptr) ? instance_ : new GameOfLifeInfiniteRenderer(renderer);
    }

    void Render(CellStateGOF state, int cell_size);
    ImVec4 background_color_ = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

private:
    explicit GameOfLifeInfiniteRenderer(SDL_Renderer* renderer);
    //~GameOfLifeInfiniteRenderer(){}
    static GameOfLifeInfiniteRenderer* instance_;
    void DrawCells(CellStateGOF state, int cell_size);
    void DrawGrid(int cell_size);
    int width_;
    int height_;

    SDL_Renderer* renderer_;

};


#endif //ROSHAN_GAMEOFLIFE_INFINITE_RENDERER_H
