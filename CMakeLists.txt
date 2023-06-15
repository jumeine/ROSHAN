cmake_minimum_required(VERSION 3.25)
project(ROSHAN)
set(CMAKE_CXX_STANDARD 17)

# Find and link against OpenGL
find_package(OpenGL REQUIRED)

# Add the imgui library
file(GLOB IMGUI_SOURCES ${CMAKE_SOURCE_DIR}/imgui/*.cpp)
add_library(imgui ${IMGUI_SOURCES} imgui/backends/imgui_impl_sdl2.cpp imgui/backends/imgui_impl_opengl3.cpp imgui/backends/imgui_impl_opengl3.h imgui/backends/imgui_impl_sdl2.h imgui/backends/imgui_impl_sdlrenderer2.cpp)
target_include_directories(imgui PUBLIC ${CMAKE_SOURCE_DIR}/imgui ${CMAKE_SOURCE_DIR}/imgui/backends/)

list(APPEND CMAKE_PREFIX_PATH "/usr/local")
include_directories(/usr/local/include/SDL2)
set(SDL2_LIBRARIES /usr/local/lib/libSDL2.so)

add_executable(ROSHAN main.cpp utils.cpp engine_core.cpp engine_core.h models/gameoflife/gameoflife_infinite.cpp models/gameoflife/gameoflife_infinite.h model_interface.h models/gameoflife/gameoflife_infinite_renderer.cpp models/gameoflife/gameoflife_infinite_renderer.h models/gameoflife/gameoflife_types.h models/gameoflife_simple/gameoflife_fixed.h models/gameoflife_simple/gameoflife_fixed_renderer.h models/gameoflife_simple/gameoflife_fixed_renderer.cpp models/gameoflife_simple/gameoflife_fixed.cpp models/stochasticlagrangian/firemodel.cpp models/stochasticlagrangian/firemodel.h models/stochasticlagrangian/firemodel_gridmap.cpp models/stochasticlagrangian/firemodel_gridmap.h models/stochasticlagrangian/firemodel_renderer.cpp models/stochasticlagrangian/firemodel_renderer.h models/stochasticlagrangian/firemodel_firecell.cpp models/stochasticlagrangian/firemodel_firecell.h point.h point_hash.h models/stochasticlagrangian/virtual_particle.cpp models/stochasticlagrangian/virtual_particle.h models/stochasticlagrangian/radiation_particle.cpp models/stochasticlagrangian/radiation_particle.h models/stochasticlagrangian/wind.cpp models/stochasticlagrangian/wind.h models/stochasticlagrangian/utils.h models/stochasticlagrangian/model_parameters.cpp models/stochasticlagrangian/model_parameters.h)
target_link_libraries(ROSHAN ${SDL2_LIBRARIES} OpenGL::GL imgui -ldl)
