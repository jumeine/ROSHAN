cmake_minimum_required(VERSION 3.25)
project(ROSHAN)
set(CMAKE_CXX_STANDARD 17)

# Replace with your actual CEF directory path
set(CEF_ROOT "/home/nex/Dokumente/libs/cef_binary_114.2.11+g87c8807+chromium-114.0.5735.134_linux64/")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")

# Execute FindCEF.cmake which must exist in CMAKE_MODULE_PATH.
find_package(CEF REQUIRED)

# Include the libcef_dll_wrapper target.
# Comes from the libcef_dll/CMakeLists.txt file in the binary distribution
# directory.
add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)

set(BROWSER_SRCS
        browser_window/simple_app.cpp browser_window/simple_app.h
        browser_window/simple_handler.cpp browser_window/simple_handler.h
        browser_window/simple_handler_linux.cpp)

ADD_LOGICAL_TARGET("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")
SET_CEF_TARGET_OUT_DIR()

## Include application targets.
## Comes from the <target>/CMakeLists.txt file in the current directory.
#if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/browser_window")
#    add_subdirectory(browser_window)
#endif()

# Find and link against OpenGL
find_package(OpenGL REQUIRED)

# Add the imgui library
file(GLOB IMGUI_SOURCES ${CMAKE_SOURCE_DIR}/imgui/*.cpp)
add_library(imgui ${IMGUI_SOURCES}
            imgui/backends/imgui_impl_sdl2.cpp
            imgui/backends/imgui_impl_opengl3.cpp
            imgui/backends/imgui_impl_opengl3.h
            imgui/backends/imgui_impl_sdl2.h
            imgui/backends/imgui_impl_sdlrenderer2.cpp)

target_include_directories(imgui PUBLIC ${CMAKE_SOURCE_DIR}/imgui ${CMAKE_SOURCE_DIR}/imgui/backends/ ${CEF_ROOT})

list(APPEND CMAKE_PREFIX_PATH "/usr/local")
include_directories(/usr/local/include/SDL2)
set(SDL2_LIBRARIES /usr/local/lib/libSDL2.so)

add_executable(ROSHAN
        main.cpp
        utils.cpp
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
        models/stochasticlagrangian/firemodel_renderer.cpp models/stochasticlagrangian/firemodel_renderer.h
        models/stochasticlagrangian/firemodel_firecell.cpp models/stochasticlagrangian/firemodel_firecell.h
        models/stochasticlagrangian/virtual_particle.cpp models/stochasticlagrangian/virtual_particle.h
        models/stochasticlagrangian/radiation_particle.cpp models/stochasticlagrangian/radiation_particle.h
        models/stochasticlagrangian/wind.cpp models/stochasticlagrangian/wind.h
        models/stochasticlagrangian/utils.h
        models/stochasticlagrangian/model_parameters.h

        ${BROWSER_SRCS})

# Link against OpenGL, imgui, SDL2, and CEF
add_dependencies(ROSHAN libcef_dll_wrapper)
target_link_libraries(ROSHAN ${SDL2_LIBRARIES} ${X11_LIBRARIES} OpenGL::GL imgui libcef_lib libcef_dll_wrapper pthread ${CEF_STANDARD_LIBS} -ldl)

set(CEF_TARGET "ROSHAN")
# Set rpath so that libraries can be placed next to the executable.
set_target_properties(${CEF_TARGET} PROPERTIES INSTALL_RPATH "$ORIGIN")
set_target_properties(${CEF_TARGET} PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
set_target_properties(${CEF_TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CEF_TARGET_OUT_DIR})

# Set rpath so that libraries can be placed next to the executable.
set_target_properties(${CEF_TARGET} PROPERTIES INSTALL_RPATH "$ORIGIN")
set_target_properties(${CEF_TARGET} PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
set_target_properties(${CEF_TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CEF_TARGET_OUT_DIR})

# Copy binary and resource files to the target output directory.
COPY_FILES("${CEF_TARGET}" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${CEF_TARGET_OUT_DIR}")
COPY_FILES("${CEF_TARGET}" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${CEF_TARGET_OUT_DIR}")
if (EXISTS "${CEF_BINARY_DIR}/libminigbm.so")
    COPY_FILES("${CEF_TARGET}" "libminigbm.so" "${CEF_BINARY_DIR}" "${CEF_TARGET_OUT_DIR}")
endif()

# Set SUID permissions on the chrome-sandbox target.
SET_LINUX_SUID_PERMISSIONS("${CEF_TARGET}" "${CEF_TARGET_OUT_DIR}/chrome-sandbox")