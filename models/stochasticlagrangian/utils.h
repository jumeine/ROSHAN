//
// Created by nex on 12.06.23.
//

#ifndef ROSHAN_UTILS_H
#define ROSHAN_UTILS_H

enum CellState { GENERIC_UNBURNED = 0,
                 SEALED = 1,
                 WOODY_NEEDLE_LEAVED_TREES = 2,
                 WOODY_BROADLEAVED_DECIDUOUS_TREES = 3,
                 WOODY_BROADLEAVED_EVERGREEN_TREES = 4,
                 LOW_GROWING_WOODY_PLANTS = 5,
                 PERMANENT_HERBACEOUS = 6,
                 PERIODICALLY_HERBACEOUS = 7,
                 LICHENS_AND_MOSSES = 8,
                 NON_AND_SPARSLEY_VEGETATED = 9,
                 WATER = 10,
                 SNOW_AND_ICE = 11,
                 OUTSIDE_AREA = 254,
                 GENERIC_BURNING = 60,
                 GENERIC_BURNED = 61};

#endif //ROSHAN_UTILS_H
