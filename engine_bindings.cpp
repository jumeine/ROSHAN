#include "engine_core.h"
#include "../externals/pybind11/include/pybind11/pybind11.h"
#include "../externals/pybind11/include/pybind11/stl.h"

namespace py = pybind11;

PYBIND11_MODULE(engine_core_module, m) {
    py::class_<EngineCore>(m, "EngineCore")
            .def(py::init<>())
            .def("GetInstance", &EngineCore::GetInstance)
            .def("Init", &EngineCore::Init)
            .def("Clean", &EngineCore::Clean)
            .def("Render", &EngineCore::Render)
            .def("HandleEvents", &EngineCore::HandleEvents)
            .def("IsRunning", &EngineCore::IsRunning)
            .def("GetRenderer", &EngineCore::GetRenderer)
            .def("ImGuiSimulationControls", &EngineCore::ImGuiSimulationControls);
}