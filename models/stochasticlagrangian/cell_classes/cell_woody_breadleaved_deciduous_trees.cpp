//
// Created by nex on 27.06.23.
//

#include "../firemodel_cell_interface.h"
#include <SDL.h>

class CellWoodyBroadleavedDeciduousTrees : public ICell {
public:
    CellWoodyBroadleavedDeciduousTrees() {
        color_ = {0, 255, 0, 255};
    }

    void Render(SDL_Renderer* renderer, const SDL_Rect& rect) override {
        SDL_SetRenderDrawColor(renderer, color_.r, color_.g, color_.b, color_.a);
        SDL_RenderFillRect(renderer, &rect);
    }
};