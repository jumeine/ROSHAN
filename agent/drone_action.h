//
// Created by nex on 25.07.23.
//

#ifndef ROSHAN_DRONE_ACTION_H
#define ROSHAN_DRONE_ACTION_H

#include "action.h"

class DroneAction : public Action{
public:
    DroneAction() : angular_(0), linear_(0) {}
    DroneAction(double angular, double linear) : angular_(angular), linear_(linear) {}
    double GetAngular() { return angular_; }
    double GetLinear() { return linear_; }
private:
    double angular_;
    double linear_;
};

#endif //ROSHAN_DRONE_ACTION_H
