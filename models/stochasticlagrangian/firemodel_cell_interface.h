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
    ImVec4 GetImVecColor() {
        return ImVec4(color_.r, color_.g, color_.b, color_.a);
    }
    Uint32 GetMappedColor() { return mapped_color_; }

protected:
    SDL_Color color_;
    Uint32 mapped_color_;

};

#endif //ROSHAN_FIREMODEL_CELL_INTERFACE_H
