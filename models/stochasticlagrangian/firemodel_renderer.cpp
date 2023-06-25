//
// Created by nex on 08.06.23.
//

#include "firemodel_renderer.h"

FireModelRenderer* FireModelRenderer::instance_ = nullptr;

void FireModelRenderer::Initialize() {
    show_grid_ = parameters_.render_grid_;
}

FireModelRenderer::FireModelRenderer(SDL_Renderer *renderer, FireModelParameters& parameters) : parameters_(parameters) {
    renderer_ = renderer;
    GetScreenResolution(width_, height_);
    Initialize();
}

void FireModelRenderer::Render() {
    SDL_RenderClear(renderer_);
    GetScreenResolution(width_, height_);
    current_cellsize_ = GetCellSize();
    DrawCells();
    if (parameters_.render_grid_ && show_grid_)
        DrawGrid();
    DrawParticles();
    // Hintergrundfarbe
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
}

void FireModelRenderer::GetScreenResolution(int& width, int& height) {
    SDL_GetRendererOutputSize(renderer_, &width, &height);
}

double FireModelRenderer::GetCellSize() {
    int gridsize = std::max(gridmap_->GetRows(), gridmap_->GetCols());

    int minwindowsize = std::min(width_, height_);

    double cellsize = minwindowsize / (double)gridsize * zoom_;

    return cellsize;
}

void FireModelRenderer::DrawCells() {

    int rows = gridmap_->GetRows();
    int cols = gridmap_->GetCols();

    double offset_x = (width_ - cols * current_cellsize_) / 2;
    double offset_y = (height_ - rows * current_cellsize_) / 2;

    // Start drawing cells from the cell at the camera position
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (i >= 0 && i < rows && j >= 0 && j < cols) {
                int cellstate = gridmap_->At(i, j).GetCellState();
                if (cellstate == GENERIC_BURNING) {
                    //SDL red
                    SDL_SetRenderDrawColor(renderer_, 255, 0, 0, 255);
                } else if (cellstate == GENERIC_BURNED) {
                    //SDL brown
                    SDL_SetRenderDrawColor(renderer_, 105, 76, 51, 255);
                } else if (cellstate == GENERIC_UNBURNED) {
                    //SDL green
                    SDL_SetRenderDrawColor(renderer_, 50, 190, 75, 255);
                } else if (cellstate == SEALED) {
                    //SDL grey
                    SDL_SetRenderDrawColor(renderer_, 100, 100, 100, 255);
                } else if (cellstate == WOODY_NEEDLE_LEAVED_TREES) {
                    //SDL dark green
                    SDL_SetRenderDrawColor(renderer_, 0, 100, 0, 255);
                } else if (cellstate == WOODY_BROADLEAVED_DECIDUOUS_TREES) {
                    //SDL very light green
                    SDL_SetRenderDrawColor(renderer_, 0, 255, 0, 255);
                } else if (cellstate == WOODY_BROADLEAVED_EVERGREEN_TREES) {
                    //SDL light green
                    SDL_SetRenderDrawColor(renderer_, 0, 180, 0, 255);
                } else if (cellstate == LOW_GROWING_WOODY_PLANTS) {
                    //SDL brown
                    SDL_SetRenderDrawColor(renderer_, 105, 76, 51, 255);
                } else if (cellstate == PERMANENT_HERBACEOUS) {
                    //SDL yellowish green
                    SDL_SetRenderDrawColor(renderer_, 173, 255, 47, 255);
                } else if (cellstate == PERIODICALLY_HERBACEOUS) {
                    //SDL light yellow
                    SDL_SetRenderDrawColor(renderer_, 255, 255, 102, 255);
                } else if (cellstate == LICHENS_AND_MOSSES) {
                    //SDL light pink
                    SDL_SetRenderDrawColor(renderer_, 255, 153, 204, 255);
                } else if (cellstate == NON_AND_SPARSLEY_VEGETATED) {
                    //SDL dark grey little green
                    SDL_SetRenderDrawColor(renderer_, 51, 51, 51, 255);
                } else if (cellstate == WATER) {
                    //SDL dark blue
                    SDL_SetRenderDrawColor(renderer_, 0, 0, 255, 255);
                } else if (cellstate == SNOW_AND_ICE) {
                    //SDL light blue
                    SDL_SetRenderDrawColor(renderer_, 0, 255, 255, 255);
                } else if (cellstate == OUTSIDE_AREA) {
                    //SDL grey black
                    SDL_SetRenderDrawColor(renderer_, 25, 25, 25, 255);
                } else {
                    //Throw error undefined state
                    throw std::runtime_error(
                            "FireModelRenderer::DrawCells: Undefined cell state at cell(" + std::to_string(i) + ", " +
                            std::to_string(j) + ")");
                }

                SDL_Rect cellRect = {
                        static_cast<int>((i + GetCamX()) * static_cast<int>(current_cellsize_) + offset_x),
                        static_cast<int>((j + GetCamY()) * static_cast<int>(current_cellsize_) + offset_y),
                        static_cast<int>(current_cellsize_),
                        static_cast<int>(current_cellsize_)
                };

                SDL_RenderFillRect(renderer_, &cellRect);
            }
        }
    }
}

void FireModelRenderer::DrawGrid() {
    int rows = gridmap_->GetRows();
    int cols = gridmap_->GetCols();

    double offset_x = (width_ - cols * current_cellsize_) / 2;
    double offset_y = (height_ - rows * current_cellsize_) / 2;

    SDL_SetRenderDrawColor(renderer_, 53, 53, 53, 255);  // color for the grid

    // Draw vertical grid lines
    int _x = static_cast<int>((GetCamX()) * static_cast<int>(current_cellsize_) + offset_x);
    for (int i = 0; i <= cols; i++) {
        int y = static_cast<int>((i + GetCamY()) * static_cast<int>(current_cellsize_) + offset_y);
        int x2 = static_cast<int>((cols + GetCamX()) * static_cast<int>(current_cellsize_) + offset_x);
        SDL_RenderDrawLine(renderer_, _x, y, x2, y);
    }

    // Draw vertical grid lines
    int _y = static_cast<int>((GetCamY()) * static_cast<int>(current_cellsize_) + offset_y);
    for (int i = 0; i <= rows; i++) {
        int x = static_cast<int>((i + GetCamX()) * static_cast<int>(current_cellsize_) + offset_x);
        int y2 = static_cast<int>((rows + GetCamY()) * static_cast<int>(current_cellsize_) + offset_y);
        SDL_RenderDrawLine(renderer_, x, _y, x, y2);
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
    int rows = gridmap_->GetRows();
    int cols = gridmap_->GetCols();

    double offset_x = (width_ - cols * current_cellsize_) / 2;
    double offset_y = (height_ - rows * current_cellsize_) / 2;

    const std::vector<RadiationParticle> particles = gridmap_->GetRadiationParticles();

    if (!particles.empty()) {
        for (const auto& particle : particles) {
            double x, y;
            double i, j;
            particle.GetPosition(x, y);
            parameters_.ConvertRealToRenderCoordinates(x, y, i, j);
            // Converting the particle positions from grid to screen space
            int posx = static_cast<int>((i + GetCamX()) * static_cast<int>(current_cellsize_) + offset_x);
            int posy = static_cast<int>((j + GetCamY()) * static_cast<int>(current_cellsize_) + offset_y);

            DrawCircle(posx, posy, static_cast<int>(current_cellsize_ / 6), particle.GetIntensity());  // Scaling with zoom
        }
    }

    const std::vector<VirtualParticle> virtual_particles = gridmap_->GetVirtualParticles();

    if (!virtual_particles.empty()) {
        for (const auto& particle : virtual_particles) {
            double x, y;
            double i, j;
            particle.GetPosition(x, y);
            parameters_.ConvertRealToRenderCoordinates(x, y, i, j);
            // Converting the particle positions from grid to screen space
            int posx = static_cast<int>((i + GetCamX()) * static_cast<int>(current_cellsize_) + offset_x);
            int posy = static_cast<int>((j + GetCamY()) * static_cast<int>(current_cellsize_) + offset_y);
            DrawCircle(posx, posy, static_cast<int>(current_cellsize_ / 6), particle.GetIntensity());  // Scaling with zoom
        }
    }
}

std::pair<int, int> FireModelRenderer::ScreenToGridPosition(int x, int y) {
    GetScreenResolution(width_, height_);

    // Get the current cell size
    double cellsize = GetCellSize();

    // Calculate offsets
    int offset_x = (width_ - gridmap_->GetCols() * cellsize) / 2;
    int offset_y = (height_ - gridmap_->GetRows() * cellsize) / 2;

    // Convert screen coordinates to grid coordinates
    int _x = static_cast<int>(((x - offset_x) / static_cast<int>(cellsize)) - GetCamX());
    int _y = static_cast<int>(((y - offset_y) / static_cast<int>(cellsize)) - GetCamY());

    return std::make_pair(_x, _y);
}

void FireModelRenderer::ApplyZoom(double z) {
    zoom_ *= z;
//    if (zoom_ < 0.4) {
//        zoom_ = 0.4;
//        show_grid_ = false;
//    } else if (zoom_ < 0.7) {
//        show_grid_ = false;
//    } else if (zoom_ > 2.0) {
//        zoom_ = 2.0;
//    } else if (zoom_ > 0.7 && zoom_ < 2.0){
//        show_grid_ = true;
//    }
}

