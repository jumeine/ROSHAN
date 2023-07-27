//
// Created by nex on 27.07.23.
//

#include "drone_state.h"
#include "../externals/pybind11/include/pybind11/pybind11.h"
#include "../externals/pybind11/include/pybind11/stl.h"

namespace py = pybind11;

PYBIND11_MODULE(drone_state_module, m) {
    py::class_<DroneState>(m, "DroneState")
            .def(py::init<>())
            .def(py::init<double, double, std::vector<std::vector<int>>, std::vector<std::vector<int>>>())
            .def("GetNewState", &DroneState::GetNewState)
            .def("SetOrientation", &DroneState::SetOrientation)
            .def("GetOrientation", &DroneState::GetOrientation)
            .def("GetNewOrientation", &DroneState::GetNewOrientation)
            .def("SetVelocity", &DroneState::SetVelocity)
            .def("GetVelocity", &DroneState::GetVelocity)
            .def("GetNewVelocity", &DroneState::GetNewVelocity)
            .def("GetTerrain", &DroneState::GetTerrain)
            .def("GetFireStatus", &DroneState::GetFireStatus);
}