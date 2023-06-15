//
// Created by nex on 13.06.23.
//

#ifndef ROSHAN_MODEL_PARAMETERS_H
#define ROSHAN_MODEL_PARAMETERS_H


class FireModelParameters {

public:
    FireModelParameters() = default;

    // Simulation parameters
    double dt_ = 0.1;
    double GetDt() const {return dt_;}
    // Minimum and maximum values for the ImGui Sliders for the simulation parameters
    double min_dt_ = 0.0001;
    double max_dt_ = 1.0;


    // Parameters for the virtual particles
    double virtualparticle_y_st_ = 1.0;
    double GetYStVirt() const {return virtualparticle_y_st_;}
    double virtualparticle_y_lim_ = 0.2;
    double GetYLimVirt() const {return virtualparticle_y_lim_;}
    double virtualparticle_fl_ = 0.7;
    double GetFlVirt() const {return virtualparticle_fl_;}
    double virtualparticle_c0_ = 1.9;
    double GetC0Virt() const {return virtualparticle_c0_;}
    double virtualparticle_tau_mem_ = 10.0; // A few tens of seconds
    double GetTauMemVirt() const {return virtualparticle_tau_mem_;}
    double Lt_ = 10.0;
    double GetLt() const {return Lt_;}
    // Minimum and maximum values for the ImGui Sliders for the virtual particles
    double tau_min = 0.1;
    double tau_max = 100.0;
    double min_Y_st_ = 0.0;
    double max_Y_st_ = 1.0;
    double min_Y_lim_ = 0.1;
    double max_Y_lim_ = 0.3;
    double min_Fl_ = 0.0;
    double max_Fl_ = 40.0;
    double min_C0_ = 1.5;
    double max_C0_ = 2.0;
    double min_Lt_ = 10.0;
    double max_Lt_ = 70.0;


    // Parameters for the radiation particles
    double radiationparticle_y_st_ = 1.0;
    double GetYStRad() const {return radiationparticle_y_st_;}
    double radiationparticle_y_lim_ = 0.2;
    double GetYLimRad() const {return radiationparticle_y_lim_;}
    double radiationparticle_Lr_ = 10.0;
    double GetLr() const {return radiationparticle_Lr_;}
    double radiationparticle_sf_0 = 0.1;
    double GetSf0() const {return radiationparticle_sf_0;}
    // Minimum and maximum values for the ImGui Sliders for the radiation particles
    double min_Lr_ = 10.0;
    double max_Lr_ = 70.0;


    // Parameters for the Cells
    double cell_ignition_threshold_ = 2;
    double GetCellIgnitionThreshold() const {return cell_ignition_threshold_;}
    double cell_ignition_time_ = 0.0;
    double GetCellIgnitionTime() const {return cell_ignition_time_;}
    double cell_burning_duration_ = 60;
    double GetCellBurningDuration() const {return cell_burning_duration_;}
    // Minimum and maximum values for the ImGui Sliders for the cells
    double min_burning_duration_ = 1.0;
    double max_burning_duration_ = 200.0;
    double min_ignition_threshold_ = 1.0;
    double max_ignition_threshold_ = 10.0;


    // Parameters for the wind
    double wind_uw_ = 0.0; // The 10-m wind speed
    double GetWindSpeed() const {return wind_uw_;}
    double wind_angle_ = 0.0; // The angle of the wind direction
    double GetAngle() const {return wind_angle_;}
    double wind_a_ = 0.2; // The component of the wind speed in the 1st direction
    double GetA() const {return wind_a_;}
    // Minimum and maximum values for the ImGui Sliders for the wind
    const double min_Uw_ = 0.0;
    const double max_Uw_ = 15.0;
    const double min_A_ = 0.2;
    const double max_A_ = 0.4;
};


#endif //ROSHAN_MODEL_PARAMETERS_H
