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
    virtual void Render(SDL_Renderer* renderer, const SDL_Rect& rect) = 0;
    ImVec4 GetColor() {
        return ImVec4(color_.r, color_.g, color_.b, color_.a);
    }

protected:
    SDL_Color color_;

};

#endif //ROSHAN_FIREMODEL_CELL_INTERFACE_H
