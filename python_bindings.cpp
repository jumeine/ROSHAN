#include "engine_core.h"
#include "externals/pybind11/include/pybind11/pybind11.h"
#include "externals/pybind11/include/pybind11/stl.h"

namespace py = pybind11;

PYBIND11_MODULE(firesim, m) {
    py::class_<Action, std::shared_ptr<Action>>(m, "Action");

    py::class_<DroneAction, Action, std::shared_ptr<DroneAction>>(m, "DroneAction")
            .def(py::init<>())
            .def(py::init<double, double>())
            .def("GetAngular", &DroneAction::GetAngular)
            .def("GetLinear", &DroneAction::GetLinear);

    py::class_<State, std::shared_ptr<State>>(m, "State");

    py::class_<DroneState, State, std::shared_ptr<DroneState>>(m, "DroneState")
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
            .def("GetFireStatus", &DroneState::GetFireStatus)
            .def_property_readonly("velocity", &DroneState::get_velocity)
            .def_property_readonly("terrain", &DroneState::get_terrain)
            .def_property_readonly("fire_status", &DroneState::get_fire_status)
            .def_property_readonly("orientation_vector", &DroneState::get_orientation_vector);

    py::class_<EngineCore>(m, "EngineCore")
            .def_static("GetInstance", &EngineCore::GetInstance)
            .def(py::init<>())
            .def("Init", &EngineCore::Init)
            .def("Clean", &EngineCore::Clean)
            .def("Render", &EngineCore::Render)
            .def("Update", &EngineCore::Update)
            .def("HandleEvents", &EngineCore::HandleEvents)
            .def("IsRunning", &EngineCore::IsRunning)
            .def("ImGuiSimulationControls", &EngineCore::ImGuiSimulationControls)
            .def("GetObservations", &EngineCore::GetObservations)
            .def("AgentIsRunning", &EngineCore::AgentIsRunning)
            .def("Step", &EngineCore::Step);
}