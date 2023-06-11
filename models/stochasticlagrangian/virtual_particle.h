//
// Created by nex on 11.06.23.
//

#ifndef ROSHAN_VIRTUAL_PARTICLE_H
#define ROSHAN_VIRTUAL_PARTICLE_H

#include <cmath>

class VirtualParticle {

public:

    VirtualParticle(int x, int y, double tau_mem_);
    void UpdateState(double u_prime, double Lt, double Uw_i, double dt);
    void RemoveParticle();
    void GetPosition(double& x1, double& x2) const;
    bool IsCapableOfIgnition() const;
    double GetTauMem() const { return tau_mem_; }
    void SetTauMem(double tau_mem) { tau_mem_ = tau_mem; }

private:

    double X_[2];        //Position
    double U_[2];        //Belocity
    double Y_st_;        //Burning status
    double tau_mem_;     // Memory timescale
    double Y_lim_;       // Ignition limit
    double Fl_;          // Scaling factor for new position
    double C0_;          // A constant close to 2
};


#endif //ROSHAN_VIRTUAL_PARTICLE_H
