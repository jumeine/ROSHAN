//
// Created by nex on 08.06.23.
//

#include <iostream>
#include "firemodel_renderer.h"

std::shared_ptr<FireModelRenderer> FireModelRenderer::instance_ = nullptr;

FireModelRenderer::FireModelRenderer(SDL_Renderer *renderer, std::shared_ptr<std::vector<std::shared_ptr<DroneAgent>>> drones, FireModelParameters& parameters) : parameters_(parameters) {
    renderer_ = renderer;
    camera_ = FireModelCamera();
    SetScreenResolution();
    drones_ = drones;
    texture_ = SDL_CreateTexture(renderer_,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STREAMING,width_,height_);

    // Load the arrow texture
    SDL_Surface *arrow_surface = IMG_Load("../assets/arrow.png");
    if (arrow_surface == NULL) {
        SDL_Log("Unable to load image: %s", SDL_GetError());
        return;
    }
    arrow_texture_ = SDL_CreateTextureFromSurface(renderer_, arrow_surface);
    SDL_FreeSurface(arrow_surface);

    // Set blend mode to the texture for transparency
    if(SDL_SetTextureBlendMode(arrow_texture_, SDL_BLENDMODE_BLEND) < 0) {
        printf("Set blend mode error: %s\n", SDL_GetError());
    }

    // Set the transparency for the texture
    if(SDL_SetTextureAlphaMod(arrow_texture_, 127) < 0) { // 127 for semi transparency, can be changed according to the needs
        printf("Set texture alpha error: %s\n", SDL_GetError());
        // Handle the error
    }

    // This whole overhead just to get the format...
    Uint32 format;
    int access, w, h;
    SDL_QueryTexture(texture_, &format, &access, &w, &h);
    pixel_format_ = SDL_AllocFormat(format);

    pixel_buffer_ = new PixelBuffer(width_, height_, parameters_.background_color_, pixel_format_);
    needs_full_redraw_ = true;
    if (texture_ == NULL) {
        SDL_Log("Unable to create texture from surface: %s", SDL_GetError());
        return;
    }
}

void FireModelRenderer::CheckCamera() {
    camera_.Update(width_, height_, gridmap_->GetRows(), gridmap_->GetCols());
}

void FireModelRenderer::Render() {
    SDL_RenderClear(renderer_);
    if (gridmap_ != nullptr) {
        camera_.Update(width_, height_, gridmap_->GetRows(), gridmap_->GetCols());
//        std::cout << "Cellsize: " << camera_.GetCellSize() << std::endl;
        DrawCells();
        DrawParticles();
        DrawDrone();
    }
}

void FireModelRenderer::ResizePixelBuffer() {
    if (pixel_buffer_ != nullptr)
        pixel_buffer_->Resize(width_, height_);
}

void FireModelRenderer::ResizeTexture() {
    if (texture_ != nullptr)
        SDL_DestroyTexture(texture_);
    texture_ = SDL_CreateTexture(renderer_,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STREAMING,width_,height_);
}

void FireModelRenderer::ResizeEvent() {
    SetScreenResolution();
    ResizePixelBuffer();
    ResizeTexture();
    SetFullRedraw();
}

void FireModelRenderer::SetScreenResolution() {
    SDL_GetRendererOutputSize(renderer_, &width_, &height_);
    camera_.SetViewport(width_, height_);
}

void FireModelRenderer::DrawCells() {

    if (needs_full_redraw_) {
        // Convert viewport corners to grid coordinates
        auto [gridLeft, gridTop] = camera_.ScreenToGridPosition(0, 0);
        auto [gridRight, gridBottom] = camera_.ScreenToGridPosition(camera_.GetViewportWidth(), camera_.GetViewportHeight());

        // Ensure grid coordinates are within gridmap bounds
        gridLeft = std::max(gridLeft, 0);
        gridTop = std::max(gridTop, 0);
        gridRight = std::min(gridRight, gridmap_->GetCols() - 1);
        gridBottom = std::min(gridBottom, gridmap_->GetRows() - 1);

        pixel_buffer_->Reset();
        DrawAllCells(gridLeft, gridRight, gridTop, gridBottom);
        needs_full_redraw_ = false;
        SDL_UpdateTexture(texture_, NULL, pixel_buffer_->GetData(), pixel_buffer_->GetPitch());
    } else {
        DrawChangesCells();
    }

    // Render the texture to the screen
    SDL_RenderCopy(renderer_, texture_, NULL, NULL);
}

void FireModelRenderer::DrawAllCells(int grid_left, int grid_right, int grid_top, int grid_bottom) {
    // Start drawing cells from the cell at the camera position
    for (int x = grid_left; x <= grid_right; ++x) {
        for (int y = grid_top; y <= grid_bottom; ++y) {
            DrawCell(x, y);
        }
    }
}

void FireModelRenderer::DrawChangesCells() {
    // Start drawing cells from the cell at the camera position
    for (const auto& cell : gridmap_->GetChangedCells()) {
        SDL_Rect cell_rect = DrawCell(cell.x_, cell.y_);
        // Get a pointer to the pixel data for the cell
        Uint32* cellPixelData = &pixel_buffer_->GetData()[cell_rect.y * pixel_buffer_->GetWidth() + cell_rect.x];

        // Update the portion of the texture that corresponds to the cell
        SDL_UpdateTexture(texture_, &cell_rect, cellPixelData, pixel_buffer_->GetPitch());
    }
    gridmap_->ResetChangedCells();
}

SDL_Rect FireModelRenderer::DrawCell(int x, int y) {
    auto [screen_x, screen_y] = camera_.GridToScreenPosition(floor(x), floor(y));
    const SDL_Rect cell_rect = {
            screen_x,
            screen_y,
            static_cast<int>(camera_.GetCellSize()),
            static_cast<int>(camera_.GetCellSize())
    };
    Uint32 color = gridmap_->At(x, y).GetMappedColor();
    if (!parameters_.render_grid_) {
        pixel_buffer_->Draw(cell_rect, color);
    } else if (camera_.GetCellSize() >= 3.0) {
        pixel_buffer_->DrawGrid(cell_rect, color);
    } else {
        pixel_buffer_->Draw(cell_rect, color);
    }

    return cell_rect;
}

void FireModelRenderer::DrawCircle(int x, int y, int min_radius, double intensity) {
    int max_radius = 3 * min_radius;
    // (intensity - 0.2) / (1.0 - 0.2)
    int radius = min_radius + static_cast<int>((max_radius - min_radius) * ((intensity - 0.2) / 0.8));
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

    int circle_radius = static_cast<int>(camera_.GetCellSize() / 6);

    if (circle_radius > 0) {
        const std::vector<RadiationParticle> particles = gridmap_->GetRadiationParticles();

        if (!particles.empty()) {
            for (const auto& particle : particles) {
                double x, y;
                particle.GetPosition(x, y);
                x = x / parameters_.GetCellSize();
                y = y / parameters_.GetCellSize();
                auto [posx, posy] = camera_.GridToScreenPosition(x, y);

                DrawCircle(posx, posy, circle_radius, particle.GetIntensity());  // Scaling with zoom
            }
        }

        const std::vector<VirtualParticle> virtual_particles = gridmap_->GetVirtualParticles();

        if (!virtual_particles.empty()) {
            for (const auto& particle : virtual_particles) {
                double x, y;
                particle.GetPosition(x, y);
                x = x / parameters_.GetCellSize();
                y = y / parameters_.GetCellSize();
                auto [posx, posy] = camera_.GridToScreenPosition(x, y);

                DrawCircle(posx, posy, circle_radius, particle.GetIntensity());  // Scaling with zoom
            }
        }
    }

}

void FireModelRenderer::DrawArrow(double angle) {
    // Render the arrow
    SDL_Rect destRect = {width_ - 100, height_ - 100, 50, 50}; // x, y, width and height of the arrow
    SDL_RenderCopyEx(renderer_, arrow_texture_, NULL, &destRect, angle, NULL, SDL_FLIP_NONE);
}

std::pair<int, int> FireModelRenderer::ScreenToGridPosition(int x, int y) {
    auto [screenX, screenY] = camera_.ScreenToGridPosition(x, y);

    return std::make_pair(screenX, screenY);
}

FireModelRenderer::~FireModelRenderer() {
    //Destroy Backbuffer
    delete pixel_buffer_;
    SDL_DestroyTexture(arrow_texture_);
    SDL_FreeFormat(pixel_format_);
    SDL_DestroyTexture(texture_);
}

void FireModelRenderer::DrawDrone() {
    double size = static_cast<int>(camera_.GetCellSize());

    for (auto &agent : *drones_) {
        std::pair<double, double> agent_position = agent->GetPosition();
        std::pair<int, int> screen_position = camera_.GridToScreenPosition(agent_position.first, agent_position.second);
        agent->Render(screen_position, size);
    }
}

