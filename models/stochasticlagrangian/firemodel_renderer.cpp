//
// Created by nex on 08.06.23.
//

#include "firemodel_renderer.h"

FireModelRenderer* FireModelRenderer::instance_ = nullptr;

void FireModelRenderer::Initialize() {

}

FireModelRenderer::FireModelRenderer(SDL_Renderer *renderer, GridMap *gridmap) {
    renderer_ = renderer;
    gridmap_ = gridmap;
    SDL_GetRendererOutputSize(renderer_, &width_, &height_);
}

void FireModelRenderer::Render(GridMap* gridmap, int cell_size) {
    SDL_RenderClear(renderer_);
    SDL_GetRendererOutputSize(renderer_, &width_, &height_);
    DrawCells(gridmap, cell_size);
    DrawGrid(gridmap, cell_size);
}

void FireModelRenderer::getWidthHeight(int& width, int& height) {
    SDL_GetRendererOutputSize(renderer_, &width, &height);
}

void FireModelRenderer::DrawCells(GridMap *gridmap, int cell_size) {

    int rows = gridmap->GetRows();
    int cols = gridmap->GetCols();
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int cellstate = gridmap->At(i, j).GetCellState();
            if (cellstate == 1) {
                //SDL red
                SDL_SetRenderDrawColor(renderer_, 255, 0, 0, 255);
            } else if (cellstate == 2) {
                //SDL grey
                SDL_SetRenderDrawColor(renderer_, 128, 128, 128, 255);
            } else {
                //SDL green
                SDL_SetRenderDrawColor(renderer_, (Uint8)(background_color_.x * 255), (Uint8)(background_color_.y * 255), (Uint8)(background_color_.z * 255), (Uint8)(background_color_.w * 255));
            }
            SDL_Rect cellRect = {i * cell_size, j * cell_size, cell_size, cell_size};
            SDL_RenderFillRect(renderer_, &cellRect);
        }
    }
}

void FireModelRenderer::DrawGrid(GridMap *gridmap, int cell_size) {
    SDL_SetRenderDrawColor(renderer_, 53, 53, 53, 255);  // color for the grid

    int rows = gridmap->GetRows();
    int cols = gridmap->GetCols();
    // Draw vertical grid lines
    for (int i = 0; i <= rows * cell_size; i += cell_size) {
        SDL_RenderDrawLine(renderer_, i, 0, i, cols * cell_size);
    }

    // Draw horizontal grid lines
    for (int i = 0; i <= cols * cell_size; i += cell_size) {
        SDL_RenderDrawLine(renderer_, 0, i, rows * cell_size, i);
    }
}
