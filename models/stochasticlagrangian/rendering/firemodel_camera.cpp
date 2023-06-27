//
// Created by nex on 27.06.23.
//

#include "firemodel_camera.h"

void FireModelCamera::Move(double dx, double dy) {
    x_ += dx * camera_speed_;
    y_ += dy * camera_speed_;
}

void FireModelCamera::Zoom(double factor) {
    zoom_ *= factor;
}

std::pair<int, int> FireModelCamera::ScreenToGridPosition(int screenX, int screenY) {

    // Convert screen coordinates to grid coordinates
    int gridX = static_cast<int>(((screenX - offset_x_) / static_cast<int>(cell_size_)) - x_);
    int gridY = static_cast<int>(((screenY - offset_y_) / static_cast<int>(cell_size_)) - y_);

    return std::make_pair(gridX, gridY);
}

std::pair<int, int> FireModelCamera::WorldToScreen(double worldX, double worldY) {

    int screenX = static_cast<int>(((worldX + x_) * static_cast<int>(cell_size_)) + offset_x_);
    int screenY = static_cast<int>(((worldY + y_) * static_cast<int>(cell_size_)) + offset_y_);

    return std::make_pair(screenX, screenY);
}

void FireModelCamera::SetCellSize(int rows, int cols, int screen_width, int screen_height) {
    cell_size_ = double(std::min(screen_width, screen_height)) / (double)std::max(rows, cols) * zoom_;
}

void FireModelCamera::SetOffset(int rows, int cols, int screen_width, int screen_height) {
    offset_x_ = (screen_width - cols * cell_size_) / 2;
    offset_y_ = (screen_height - rows * cell_size_) / 2;
}
