//
// Created by nex on 01.08.23.
//

#include "drone_action.h"
#include "../externals/pybind11/include/pybind11/pybind11.h"
#include "../externals/pybind11/include/pybind11/stl.h"

namespace py = pybind11;

PYBIND11_MODULE(drone_action_module, m) {
    py::class_<DroneAction>(m, "DroneAction")
            .def(py::init<>())
            .def(py::init<double, double>())
            .def("GetAngular", &DroneAction::GetAngular)
            .def("GetLinear", &DroneAction::GetLinear);
}