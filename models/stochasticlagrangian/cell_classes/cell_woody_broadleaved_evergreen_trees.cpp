//
// Created by nex on 27.06.23.
//

#include "../firemodel_cell_interface.h"
#include <SDL.h>

class CellWoodyBroadleavedEvergreenTrees : public ICell {
public:
    CellWoodyBroadleavedEvergreenTrees(SDL_PixelFormat* format) {
        color_ = {0, 180, 0, 255};
        mapped_color_ = SDL_MapRGBA(format, color_.r, color_.g, color_.b, color_.a);
    }
};