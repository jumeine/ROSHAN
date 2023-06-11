//
// Created by nex on 11.06.23.
//

#include "wind.h"

void Wind::UpdateWind(double new_Uw_i, double new_A) {
    Uw_i_ = new_Uw_i;
    A_ = new_A;
    u_prime_ = A_ * Uw_i_;
}

Wind::Wind(double Uw_i, double A) {
    Uw_i_ = Uw_i;
    A_ = A;
    u_prime_ = A_ * Uw_i_;
}
