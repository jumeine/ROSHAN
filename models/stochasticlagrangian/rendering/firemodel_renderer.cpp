//
// Created by nex on 08.06.23.
//

#include <iostream>
#include "firemodel_renderer.h"

FireModelRenderer* FireModelRenderer::instance_ = nullptr;

FireModelRenderer::FireModelRenderer(SDL_Renderer *renderer, FireModelParameters& parameters) : parameters_(parameters) {
    renderer_ = renderer;
    camera_ = FireModelCamera();
    GetScreenResolution(width_, height_);
    drawing_surface_ = SDL_CreateRGBSurfaceWithFormat(0, width_, height_, 32, SDL_PIXELFORMAT_ARGB8888);
    back_buffer_ = SDL_CreateTexture(renderer_,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,width_,height_);
    pixel_buffer_ = new Uint32[width_ * height_];
    texture_ = SDL_CreateTexture(renderer_,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STREAMING,width_,height_);
    if (texture_ == NULL) {
        SDL_Log("Unable to create texture from surface: %s", SDL_GetError());
        return;
    }
}

void FireModelRenderer::Render() {
    SDL_RenderClear(renderer_);
    if (gridmap_ != nullptr) {
        GetScreenResolution(width_, height_);
        camera_.SetViewport(width_, height_);
        camera_.SetCellSize(gridmap_->GetRows(), gridmap_->GetCols());
        camera_.SetOffset(gridmap_->GetRows(), gridmap_->GetCols());
        DrawCells();
        if (parameters_.render_grid_)
            DrawGrid();
        DrawParticles();
    }
    // Hintergrundfarbe
    SDL_SetRenderDrawColor(renderer_, 41, 49, 51, 255);
}

void FireModelRenderer::GetScreenResolution(int& width, int& height) {
    SDL_GetRendererOutputSize(renderer_, &width, &height);
}

void FireModelRenderer::DrawCells() {

    int rows = gridmap_->GetRows();
    int cols = gridmap_->GetCols();

    // Clear the surface
    SDL_memset(drawing_surface_->pixels, 0, drawing_surface_->h * drawing_surface_->pitch);

    // Start drawing cells from the cell at the camera position
    for (int x = 0; x < cols; ++x) {
        for (int y = 0; y < rows; ++y) {
            auto [screen_x, screen_y] = camera_.WorldToScreen(x, y);
            SDL_Rect cellRect = {
                    screen_x,
                    screen_y,
                    static_cast<int>(camera_.GetCellSize()),
                    static_cast<int>(camera_.GetCellSize())
            };
            gridmap_->At(x, y).Render(drawing_surface_, cellRect);
        }
    }

    // Update the texture
    SDL_UpdateTexture(texture_, NULL, drawing_surface_->pixels, drawing_surface_->pitch);

    // Render the texture to the screen
    SDL_RenderCopy(renderer_, texture_, NULL, NULL);
}

void FireModelRenderer::DrawGrid() {
    int rows = gridmap_->GetRows();
    int cols = gridmap_->GetCols();

    int cell_size = static_cast<int>(floor(camera_.GetCellSize()));

    // Set color for the grid (light grey)
    SDL_SetRenderDrawColor(renderer_, 53, 53, 53, 255);  // color for the grid
    auto [x0, y0] = camera_.WorldToScreen(0, 0);
    auto [x_end, y_end] = camera_.WorldToScreen(cols, rows);

    // Draw horizontal grid lines
    for (int i = 0; i <= rows; i++) {
        int y = y0 + i * cell_size;
        SDL_RenderDrawLine(renderer_, x0, y, x_end, y);
    }

    // Draw vertical grid lines
    for (int i = 0; i <= cols; i++) {
        int x = x0  + i * cell_size;
        SDL_RenderDrawLine(renderer_, x, y0, x, y_end);
    }
}

void FireModelRenderer::DrawCircle(int x, int y, int min_radius, double intensity) {
    int max_radius = 3 * min_radius;
    int radius = min_radius + static_cast<int>((max_radius - min_radius) * ((intensity - 0.2) / (1.0 - 0.2)));
    unsigned char g = static_cast<int>(255 * ((intensity - 0.2) / (1.0 - 0.2)));
    SDL_Color color = {255, g, 0, 255};

    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
    for(int w = 0; w < radius * 2; w++) {
        for(int h = 0; h < radius * 2; h++) {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if((dx*dx + dy*dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer_, x + dx, y + dy);
            }
        }
    }
}

void FireModelRenderer::DrawParticles() {

    const std::vector<RadiationParticle> particles = gridmap_->GetRadiationParticles();

    if (!particles.empty()) {
        for (const auto& particle : particles) {
            double x, y;
            particle.GetPosition(x, y);
            x = x / parameters_.GetCellSize();
            y = y / parameters_.GetCellSize();
            auto [posx, posy] = camera_.WorldToScreen(x, y);

            DrawCircle(posx, posy, static_cast<int>(camera_.GetCellSize() / 6), particle.GetIntensity());  // Scaling with zoom
        }
    }

    const std::vector<VirtualParticle> virtual_particles = gridmap_->GetVirtualParticles();

    if (!virtual_particles.empty()) {
        for (const auto& particle : virtual_particles) {
            double x, y;
            particle.GetPosition(x, y);
            x = x / parameters_.GetCellSize();
            y = y / parameters_.GetCellSize();
            auto [posx, posy] = camera_.WorldToScreen(x, y);

            DrawCircle(posx, posy, static_cast<int>(camera_.GetCellSize() / 6), particle.GetIntensity());  // Scaling with zoom
        }
    }
}

std::pair<int, int> FireModelRenderer::ScreenToGridPosition(int x, int y) {

    GetScreenResolution(width_, height_);
    auto [screenX, screenY] = camera_.ScreenToGridPosition(x, y);

    return std::make_pair(screenX, screenY);
}

FireModelRenderer::~FireModelRenderer() {
    //Destroy Backbuffer
    SDL_DestroyTexture(back_buffer_);
    SDL_FreeSurface(drawing_surface_);
    SDL_DestroyTexture(texture_);
}

