//
// Created by nex on 11.06.23.
//

#ifndef ROSHAN_WIND_H
#define ROSHAN_WIND_H


class Wind {

public:

    Wind(double Uw_i, double A);

    void UpdateWind(double new_Uw_i, double new_A);
    double GetWindSpeed() const {return Uw_i_;}
    double GetA() const {return A_;}
    double GetTurbulece() const {return u_prime_;}
    const double min_Uw_i_ = 0.0;
    const double max_Uw_i_ = 15.0;
    const double min_A_ = 0.0;
    const double max_A_ = 10.0;

private:
    double Uw_i_; // Wind speed in the ith direction
    double A_; // Parameter related to turbulent velocity fluctuations
    double u_prime_; // Turbulent velocity fluctuations
};


#endif //ROSHAN_WIND_H
