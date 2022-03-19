cmake_minimum_required(VERSION 3.0.0)
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
set(Engine_include_dir ${CMAKE_SOURCE_DIR}/src/Engine)

# find path to libraries
find_package(SDL2 REQUIRED)
find_package(NOUD REQUIRED)
find_package(GLM REQUIRED)
find_package(STB REQUIRED)
find_package(VMA REQUIRED)
find_package(TOL REQUIRED)
find_package(Vulkan REQUIRED)

# shaders
add_custom_target(build_shaders ALL
    COMMAND "${CMAKE_SOURCE_DIR}/compileShader.bat"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

include_directories(${SDL2_INCLUDE_DIRS} ${NOUD_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS} ${Vulkan_INCLUDE_DIRS} ${Engine_include_dir} ${STB_INCLUDE_DIRS} ${VMA_INCLUDE_DIRS} ${TOL_INCLUDE_DIRS})

add_executable(LyraEngine 
include/NOUD/noud.cpp
src/Engine/init/init_SDL.cpp 
src/Engine/init/single_header_init.cpp
src/Engine/math/math.cpp
src/Engine/core/queue_types.cpp
src/Engine/rendering/vulkan_wrappers.cpp
src/Engine/rendering/camera.cpp
src/Engine/rendering/window.cpp
src/Engine/rendering/renderer.cpp
src/Engine/res/loaders.cpp
src/Engine/res/mesh.cpp
src/Engine/res/texture.cpp
src/GUI/main.cpp 
)

add_dependencies(LyraEngine build_shaders)

target_link_libraries(LyraEngine ${SDL2_LIBRARIES} Vulkan::Vulkan)
