cmake_minimum_required(VERSION 3.16.0)
project(LyraEngine VERSION 0.1.0)

# set default to debug
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)

set(BIN_DIR ${CMAKE_SOURCE_DIR}/build)

# directories for the config files
set(SDL2_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(NOUD_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(GLM_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(STB_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(VMA_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(TOL_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(ENGINE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/Engine)
set(GUI_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/GUI)

# find path to libraries
find_package(SDL2 REQUIRED)
find_package(NOUD REQUIRED)
find_package(GLM REQUIRED)
find_package(STB REQUIRED)
find_package(VMA REQUIRED)
find_package(TOL REQUIRED)
find_package(IMGUI REQUIRED)
find_package(Vulkan REQUIRED)

# shaders
add_custom_target(build_shaders ALL
    COMMAND "${CMAKE_SOURCE_DIR}/compileShader.bat"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

include_directories(
 ${SDL2_INCLUDE_DIRS} 
 ${NOUD_INCLUDE_DIRS} 
 ${GLM_INCLUDE_DIRS} 
 ${Vulkan_INCLUDE_DIRS} 
 ${ENGINE_INCLUDE_DIR}
 ${GUI_INCLUDE_DIR}
 ${STB_INCLUDE_DIRS} 
 ${VMA_INCLUDE_DIRS} 
 ${TOL_INCLUDE_DIRS} 
 ${IMGUI_INCLUDE_DIRS}
)

add_executable(LyraEngine 
 "include/imgui/imgui.cpp"          # imgui
 "include/imgui/imgui_demo.cpp"
 "include/imgui/imgui_draw.cpp"
 "include/imgui/imgui_tables.cpp"
 "include/imgui/imgui_widgets.cpp"
 "include/imgui/backends/imgui_impl_vulkan.cpp"
 "include/imgui/backends/imgui_impl_sdl.cpp"
 "include/NOUD/noud.cpp"            # noud
 "src/Engine/init/init_SDL.cpp"     # engine
 "src/Engine/init/single_header_init.cpp"
 "src/Engine/math/math.cpp"
 "src/Engine/graphics/camera.cpp"
 "src/Engine/graphics/window.cpp"
 "src/Engine/graphics/renderer.cpp"
 "src/Engine/graphics/pipeline.cpp"
 "src/Engine/graphics/mesh.cpp"
 "src/Engine/graphics/texture.cpp"
 "src/Engine/core/queue_types.cpp"
 "src/Engine/core/rendering/context.cpp"
 "src/Engine/core/rendering/vulkan_core_wrappers.cpp"
 "src/Engine/res/loaders.cpp"
 "src/main.cpp"
 "src/GUI/core/gui_context.cpp"     # engine GUI
)

add_dependencies(LyraEngine build_shaders)

target_link_libraries(LyraEngine ${SDL2_LIBRARIES} Vulkan::Vulkan)

target_precompile_headers(LyraEngine
PRIVATE
    <vector>
    <iostream>
    <fstream>
    <string>
    <unordered_map>
    <array>
    <math.h>
    <algorithm>
)
