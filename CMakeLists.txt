cmake_minimum_required(VERSION 3.25)
project(ROSHAN)
set(CMAKE_CXX_STANDARD 17)

# set cmake module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Find and link against OpenGL
find_package(OpenGL REQUIRED)

# Add the imgui library
file(GLOB IMGUI_SOURCES ${CMAKE_SOURCE_DIR}/externals/imgui/*.cpp)
add_library(imgui_external ${IMGUI_SOURCES}
        externals/imgui/backends/imgui_impl_sdl2.cpp
        externals/imgui/backends/imgui_impl_opengl3.cpp
        externals/imgui/backends/imgui_impl_opengl3.h
        externals/imgui/backends/imgui_impl_sdl2.h
        externals/imgui/backends/imgui_impl_sdlrenderer2.cpp agent/drone.cpp agent/drone.h)

#add_library(imgui_filedialog ${CMAKE_SOURCE_DIR}/externals/ImGuiFileDialog/ImGuiFileDialog.cpp)

target_include_directories(imgui_external PUBLIC ${CMAKE_SOURCE_DIR}/externals/imgui ${CMAKE_SOURCE_DIR}/externals/imgui/backends/)
#target_include_directories(imgui_filedialog PUBLIC ${CMAKE_SOURCE_DIR}/externals/ImGuiFileDialog/)

# GDAL
find_package(GDAL REQUIRED)
include_directories(${GDAL_INCLUDE_DIRS})

# SDL2_images
#find_package(SDL2 REQUIRED)
#find_package(SDL2_image REQUIRED)
#include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})
set(SDL2_IMAGE_LIBRARIES "/usr/local/lib/libSDL2_image.so")

# SDL2
list(APPEND CMAKE_PREFIX_PATH "/usr/local")

include_directories(/usr/local/include/SDL2)
set(SDL2_LIBRARIES /usr/local/lib/libSDL2.so)



add_executable(ROSHAN
        main.cpp
        models/stochasticlagrangian/utils.cpp
        model_interface.h
        point.h
        point_hash.h
        engine_core.cpp engine_core.h

        models/gameoflife/gameoflife_infinite.cpp models/gameoflife/gameoflife_infinite.h
        models/gameoflife/gameoflife_infinite_renderer.cpp models/gameoflife/gameoflife_infinite_renderer.h
        models/gameoflife/gameoflife_types.h


        models/gameoflife_simple/gameoflife_fixed.cpp models/gameoflife_simple/gameoflife_fixed.h
        models/gameoflife_simple/gameoflife_fixed_renderer.cpp models/gameoflife_simple/gameoflife_fixed_renderer.h


        models/stochasticlagrangian/firemodel.cpp models/stochasticlagrangian/firemodel.h
        models/stochasticlagrangian/firemodel_gridmap.cpp models/stochasticlagrangian/firemodel_gridmap.h
        models/stochasticlagrangian/rendering/firemodel_renderer.cpp models/stochasticlagrangian/rendering/firemodel_renderer.h
        models/stochasticlagrangian/rendering/firemodel_camera.cpp models/stochasticlagrangian/rendering/firemodel_camera.h
        models/stochasticlagrangian/firemodel_firecell.cpp models/stochasticlagrangian/firemodel_firecell.h
        models/stochasticlagrangian/virtual_particle.cpp models/stochasticlagrangian/virtual_particle.h
        models/stochasticlagrangian/radiation_particle.cpp models/stochasticlagrangian/radiation_particle.h
        models/stochasticlagrangian/wind.cpp models/stochasticlagrangian/wind.h
        models/stochasticlagrangian/utils.h
        models/stochasticlagrangian/model_parameters.h
        CORINE/dataset_handler.cpp CORINE/dataset_handler.h
        models/stochasticlagrangian/firemodel_cell_interface.h
            models/stochasticlagrangian/cell_classes/cell_generic_unburned.cpp
            models/stochasticlagrangian/cell_classes/cell_sealed.cpp
            models/stochasticlagrangian/cell_classes/cell_woody_needle_leaved_trees.cpp
            models/stochasticlagrangian/cell_classes/cell_woody_breadleaved_deciduous_trees.cpp
            models/stochasticlagrangian/cell_classes/cell_woody_broadleaved_evergreen_trees.cpp
            models/stochasticlagrangian/cell_classes/cell_low_growing_woody_plants.cpp
            models/stochasticlagrangian/cell_classes/cell_permanent_herbaceous.cpp
            models/stochasticlagrangian/cell_classes/cell_periodically_herbaceous.cpp
            models/stochasticlagrangian/cell_classes/cell_lichens_and_mosses.cpp
            models/stochasticlagrangian/cell_classes/cell_non_and_sparsley_vegetated.cpp
            models/stochasticlagrangian/cell_classes/cell_water.cpp
            models/stochasticlagrangian/cell_classes/cell_snow_and_ice.cpp
            models/stochasticlagrangian/cell_classes/cell_generic_burning.cpp
            models/stochasticlagrangian/cell_classes/cell_outside_area.cpp
            models/stochasticlagrangian/cell_classes/cell_generic_burned.cpp

            externals/ImGuiFileDialog/ImGuiFileDialog.cpp externals/ImGuiFileDialog/ImGuiFileDialog.h
        models/stochasticlagrangian/rendering/firemodel_pixelbuffer.cpp models/stochasticlagrangian/rendering/firemodel_pixelbuffer.cpp models/stochasticlagrangian/rendering/firemodel_pixelbuffer.h agent/rendering/DroneRenderer.cpp agent/rendering/DroneRenderer.h agent/drone_state.cpp agent/drone_state.h)

target_include_directories(ROSHAN PRIVATE ${CMAKE_SOURCE_DIR}/externals/nlohmann)
target_include_directories(ROSHAN PRIVATE ${CMAKE_SOURCE_DIR})
# Link against OpenGL, imgui, SDL2
target_link_libraries(ROSHAN ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${X11_LIBRARIES} OpenGL::GL imgui_external ${GDAL_LIBRARIES} -ldl)