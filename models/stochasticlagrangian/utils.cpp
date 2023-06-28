#include "utils.h"

std::string CellStateToString(CellState cell_state) {
    switch (cell_state) {
        case GENERIC_UNBURNED:
            return "GENERIC_UNBURNED";
        case SEALED:
            return "SEALED";
        case WOODY_NEEDLE_LEAVED_TREES:
            return "WOODY_NEEDLE_LEAVED_TREES";
        case WOODY_BROADLEAVED_DECIDUOUS_TREES:
            return "WOODY_BROADLEAVED_DECIDUOUS_TREES";
        case WOODY_BROADLEAVED_EVERGREEN_TREES:
            return "WOODY_BROADLEAVED_EVERGREEN_TREES";
        case LOW_GROWING_WOODY_PLANTS:
            return "LOW_GROWING_WOODY_PLANTS";
        case PERMANENT_HERBACEOUS:
            return "PERMANENT_HERBACEOUS";
        case PERIODICALLY_HERBACEOUS:
            return "PERIODICALLY_HERBACEOUS";
        case LICHENS_AND_MOSSES:
            return "LICHENS_AND_MOSSES";
        case NON_AND_SPARSLEY_VEGETATED:
            return "NON_AND_SPARSLEY_VEGETATED";
        case WATER:
            return "WATER";
        case SNOW_AND_ICE:
            return "SNOW_AND_ICE";
        case OUTSIDE_AREA:
            return "OUTSIDE_AREA";
        case GENERIC_BURNING:
            return "GENERIC_BURNING";
        case GENERIC_BURNED:
            return "GENERIC_BURNED";
        default:
            return "UNKNOWN";
    }
}
