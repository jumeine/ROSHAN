//
// Created by nex on 06.06.23.
//

#ifndef ROSHAN_UTILS_H
#define ROSHAN_UTILS_H


#include <SDL.h>
#include <vector>
#include <random>
#include <chrono>

extern const int WIDTH;
extern const int HEIGHT;
extern const int CELL_SIZE;

extern std::vector<std::vector<bool>> cells;

void drawCells(SDL_Renderer* renderer);
void drawGrid(SDL_Renderer* renderer);
void randomizeCells();
void resetCells();
void updateCells();


#endif //ROSHAN_UTILS_H
