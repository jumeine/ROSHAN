//
// Created by nex on 13.06.23.
//

#ifndef ROSHAN_MODEL_PARAMETERS_H
#define ROSHAN_MODEL_PARAMETERS_H


class FireModelParameters {

public:
    FireModelParameters() = default;

    //Render parameters
    bool render_grid_ = true;

    // Simulation parameters
    double dt_ = 1; // in seconds (s)
    double GetDt() const {return dt_;}
    // Minimum and maximum values for the ImGui Sliders for the simulation parameters
    double min_dt_ = 0.0001; // in seconds (s)
    double max_dt_ = 1.0; // in seconds (s)


    // Parameters for the virtual particles
    double virtualparticle_y_st_ = 1.0; // Hotness of the Particle (no real world unit yet)
    double GetYStVirt() const {return virtualparticle_y_st_;}
    double virtualparticle_y_lim_ = 0.2;  // How long the Particle is able to cause ignition (no real world unit yet)
    double GetYLimVirt() const {return virtualparticle_y_lim_;}
    double virtualparticle_fl_ = 0.15;  // Scaling factor for new position (need to calibrate)
    double GetFlVirt() const {return virtualparticle_fl_;}
    double virtualparticle_c0_ = 1.98; // A constant close to 2
    double GetC0Virt() const {return virtualparticle_c0_;}
    double virtualparticle_tau_mem_ = 10.0; // A few tens of seconds
    double GetTauMemVirt() const {return virtualparticle_tau_mem_;}
    double Lt_ = 50.0; // Height of emitting source (m)
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
    double radiationparticle_Lr_ = 10.0; // Height of emmission source (m)
    double GetLr() const {return radiationparticle_Lr_;}
    double radiationparticle_sf_0 = 0.06; // Zero Wind propagation m/s
    double GetSf0() const {return radiationparticle_sf_0;}
    // Minimum and maximum values for the ImGui Sliders for the radiation particles
    double min_Lr_ = 10.0;
    double max_Lr_ = 70.0;

    // Parameter for the Grid
    // Number of cells in the x direction (rows)
    int grid_nx_ = 100;
    int GetGridNx() const {return grid_nx_;}
    // Number of cells in the y direction (cols)
    int grid_ny_ = 100;
    int GetGridNy() const {return grid_ny_;}


    // Parameters for the Cells

    // Time that the cell needs to be visited by a particle to be ignited
    double cell_ignition_threshold_ = 10; // in seconds (s)
    double GetIgnitionDelayTime() const {return cell_ignition_threshold_;}
    // Initial time the cell was visited by a particle typically 0.0
    double cell_ignition_time_ = 0.0;
    double GetCellIgnitionTime() const {return cell_ignition_time_;}
    // Time the cell is capable of burning
    double cell_burning_duration_ = 60; // in seconds (s)
    double GetCellBurningDuration() const {return cell_burning_duration_;}
    // We assume quadratic cells and this is the length of the side of the cell
    double cell_size_ = 2.0; // in meters (m)
    double GetCellSize() const {return cell_size_;} // in meters (m)
    // Minimum and maximum values for the ImGui Sliders for the cells
    double min_burning_duration_ = 1.0;
    double max_burning_duration_ = 200.0;
    double min_ignition_threshold_ = 1.0;
    double max_ignition_threshold_ = 60.0;


    // Parameters for the wind
    double wind_uw_ = 10.0; // The 10-m wind speed in m/s
    double GetWindSpeed() const {return wind_uw_;}
    double wind_angle_ = 0.0; // The angle of the wind direction
    double GetAngle() const {return wind_angle_;}
    double wind_a_ = 0.3; // The component of the wind speed in the 1st direction
    double GetA() const {return wind_a_;}
    // Minimum and maximum values for the ImGui Sliders for the wind
    const double min_Uw_ = 0.0;
    const double max_Uw_ = 15.0;
    const double min_A_ = 0.2;
    const double max_A_ = 0.4;

    void ConvertRealToGridCoordinates(double x, double y, int &i, int &j) {
        i = int(x / GetCellSize());
        j = int(y / GetCellSize());
    }

    void ConvertRealToRenderCoordinates(double x, double y, double &i, double &j) {
        i = x / GetCellSize();
        j = y / GetCellSize();
    }

};


#endif //ROSHAN_MODEL_PARAMETERS_H
