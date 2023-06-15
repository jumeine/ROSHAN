//
// Created by nex on 08.06.23.
//

#include "firemodel_renderer.h"

FireModelRenderer* FireModelRenderer::instance_ = nullptr;

void FireModelRenderer::Initialize() {

}

FireModelRenderer::FireModelRenderer(SDL_Renderer *renderer) {
    renderer_ = renderer;
    SDL_GetRendererOutputSize(renderer_, &width_, &height_);
}

void FireModelRenderer::Render(GridMap* gridmap, int cell_size) {
    SDL_RenderClear(renderer_);
    SDL_GetRendererOutputSize(renderer_, &width_, &height_);
    DrawCells(gridmap, cell_size);
    DrawGrid(gridmap, cell_size);
    DrawParticles(cell_size);
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
            if (cellstate == BURNING) {
                //SDL red
                SDL_SetRenderDrawColor(renderer_, 255, 0, 0, 255);
            } else if (cellstate == BURNED) {
                //SDL brown
                SDL_SetRenderDrawColor(renderer_, 105, 76, 51, 255);
            } else if (cellstate == UNBURNED){
                //SDL green
                SDL_SetRenderDrawColor(renderer_, (Uint8)(background_color_.x * 255), (Uint8)(background_color_.y * 255), (Uint8)(background_color_.z * 255), (Uint8)(background_color_.w * 255));
            } else {
                //Throw error undefined state
                throw std::runtime_error("FireModelRenderer::DrawCells: Undefined cell state at cell(" + std::to_string(i) + ", " + std::to_string(j) + ")");
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

void FireModelRenderer::DrawCircle(int x, int y, int radius) {
    SDL_SetRenderDrawColor(renderer_, 255, 255, 0, 255); // yellow
    for(int w = 0; w < radius * 2; w++)
    {
        for(int h = 0; h < radius * 2; h++)
        {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if((dx*dx + dy*dy) <= (radius * radius))
            {
                SDL_RenderDrawPoint(renderer_, x + dx, y + dy);
            }
        }
    }
}

void FireModelRenderer::DrawParticles(int cell_size) {
    // Set color for particles (let's say yellow)
    SDL_SetRenderDrawColor(renderer_, 255, 255, 0, 255);

    const std::vector<RadiationParticle> particles = gridmap_->GetRadiationParticles();

    if (!particles.empty()) {
        for (const auto& particle : particles) {
            double x, y;
            particle.GetPosition(x, y);
            int posx = (static_cast<int>(x) * cell_size) + cell_size / 2;
            int posy = (static_cast<int>(y) * cell_size) + cell_size / 2;
            DrawCircle(posx, posy, 1);
        }
    }

    const std::vector<VirtualParticle> fire_particles = gridmap_->GetVirtualParticles();

    if (!fire_particles.empty()) {
        for (const auto& particle : fire_particles) {
            double x, y;
            particle.GetPosition(x, y);
            int posx = (static_cast<int>(x) * cell_size) + cell_size / 2;
            int posy = (static_cast<int>(y) * cell_size) + cell_size / 2;
            DrawCircle(posx, posy, 1);
        }
    }

}
