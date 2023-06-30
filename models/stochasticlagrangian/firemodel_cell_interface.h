//
// Created by nex on 27.06.23.
//

#ifndef ROSHAN_FIREMODEL_CELL_INTERFACE_H
#define ROSHAN_FIREMODEL_CELL_INTERFACE_H

#include <SDL.h>
#include "imgui.h"
#include "utils.h"

class ICell {
public:
    virtual ~ICell() = default;
    void Render(SDL_Surface* surface, const SDL_Rect& rect) {
        DrawOnSurface(surface, &rect);
//        SDL_FillRect(surface, &rect, mapped_color_);
    }
    ImVec4 GetColor() {
        return ImVec4(color_.r, color_.g, color_.b, color_.a);
    }
    void DrawOnSurface(SDL_Surface* surface, const SDL_Rect *rect) {
        // Only draw within the surface.
        int x_end = rect->x + rect->w > surface->w ? surface->w : rect->x + rect->w;
        int y_end = rect->y + rect->h > surface->h ? surface->h : rect->y + rect->h;

        for (int y = rect->y; y < y_end; ++y) {
            for (int x = rect->x; x < x_end; ++x) {
                ((Uint32*)surface->pixels)[(y * surface->w) + x] = mapped_color_;
            }
        }
    }

protected:
    SDL_Color color_;
    Uint32 mapped_color_;

};

#endif //ROSHAN_FIREMODEL_CELL_INTERFACE_H
