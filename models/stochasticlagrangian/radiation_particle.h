//
// Created by nex on 11.06.23.
//

#ifndef ROSHAN_RADIATION_PARTICLE_H
#define ROSHAN_RADIATION_PARTICLE_H

#include <cmath>

class RadiationParticle {

public:
    RadiationParticle(double x, double y, double Lr, double Sf_0, double Y_st, double Y_lim);
    void UpdateState(double dt);
    void GetPosition(double &x1, double &x2) const;
    double GetIntensity() const { return Y_st_; }
    void RemoveParticle();
    bool IsCapableOfIgnition() const { return Y_st_ >= Y_lim_; }

private:

    double X_[2];         // Position
    double phi_r_;        // Angle
    double Y_st_;         // Burning status
    double Y_lim_;        // Ignition limit
    double tau_mem_;      // Decay timescale
    double Lr_; //10;      // A const Scaling factor
    double Sf_0_;    // A constant which represents the no-wind propagation speed

};


#endif //ROSHAN_RADIATION_PARTICLE_H
