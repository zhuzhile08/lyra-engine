cmake_minimum_required(VERSION 3.22.0)
project(LyraEngine VERSION 0.5.0)

# set default to debug
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_STANDARD 17)

# directories for the config files
set(SDL2_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(NOUD_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(GLM_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(STB_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(VMA_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(TOL_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(ENGINE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/)

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

# building the library
add_library(LyraEngine
 "../include/imgui/imgui.cpp"                  # imgui
 "../include/imgui/imgui_demo.cpp"
 "../include/imgui/imgui_draw.cpp"
 "../include/imgui/imgui_tables.cpp"
 "../include/imgui/imgui_widgets.cpp"
 "../include/imgui/backends/imgui_impl_vulkan.cpp"
 "../include/imgui/backends/imgui_impl_sdl.cpp"
 "../include/NOUD/noud.cpp"                    # noud
 "src/init/init_SDL.cpp"                    # engine
 "src/init/single_header_init.cpp"
 "src/math/math.cpp"
 "src/math/vulkan/compute_pipeline.cpp"
 "src/components/graphics/camera.cpp"
 "src/core/rendering/window.cpp"
 "src/core/rendering/gui_context.cpp"       # engine GUI
  
 "src/graphics/renderer.cpp"
 "src/graphics/graphics_pipeline.cpp"
 "src/components/mesh/mesh.cpp"
 "src/graphics/texture.cpp"
 "src/core/rendering/context.cpp"
 "src/core/rendering/vulkan_core_wrappers.cpp"
 "src/res/loaders.cpp"
 "src/lyra.cpp"
 "src/core/settings.cpp"
 "src/graphics/asset_manager.cpp" "src/core/logger.cpp"
  "src/components/gameObj.cpp")

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
    <functional>
)
