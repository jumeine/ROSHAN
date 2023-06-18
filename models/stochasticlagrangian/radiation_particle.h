//
// Created by nex on 11.06.23.
//

#ifndef ROSHAN_RADIATION_PARTICLE_H
#define ROSHAN_RADIATION_PARTICLE_H

#include <cmath>
#include <random>

class RadiationParticle {

public:
    RadiationParticle(double x, double y, double Lr, double Sf_0, double Y_st, double Y_lim, std::mt19937 gen);
    void UpdateState(double dt);
    void GetPosition(double &x1, double &x2) const;
    double GetIntensity() const { return Y_st_; }
    void RemoveParticle();
    bool IsCapableOfIgnition() const { return Y_st_ >= Y_lim_; }

private:

//    double X_[2];         // Position
//    double phi_r_;        // Angle
//    double Y_st_;         // Burning status
//    double Y_lim_;        // Ignition limit
//    double tau_mem_;      // Decay timescale
//    double Lr_; //10;      // A const Scaling factor
//    double Sf_0_;    // A constant which represents the no-wind propagation speed

    double X_[2];         // Position
    double X_mc_[2];      // Position of the mother cell
    double r_p_;          // Radius
    double phi_p_;        // Angle
    double Y_st_;         // Burning status
    double Y_lim_;        // Ignition limit
    double tau_mem_;      // Decay timescale
    double Lr_;           // A const Scaling factor
    double Sf_0_mean_;
    double Sf_0_std_;
    double Sf_0_;         // A normal distribution

    // Generate a normally-distributed random number for phi_r
    std::mt19937 gen_;

};


#endif //ROSHAN_RADIATION_PARTICLE_H
