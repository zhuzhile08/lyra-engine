cmake_minimum_required(VERSION 3.22.0)
project(ImGui VERSION 1.8.8)

# set default to debug
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_STANDARD 17)

# directories for the config files
set(SDL2_DIR ${CMAKE_SOURCE_DIR}/cmake)

# find path to libraries
find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)

# includes
include_directories(
	${SDL2_INCLUDE_DIRS} 
	${GLM_INCLUDE_DIRS} 
	${Vulkan_INCLUDE_DIRS} 
	${ENGINE_INCLUDE_DIR}
	${GUI_INCLUDE_DIR}
	${STB_INCLUDE_DIRS}
	${VMA_INCLUDE_DIRS} 
	${TOL_INCLUDE_DIRS} 
	${JSON_INCLUDE_DIRS}
	${LZ4_INCLUDE_DIRS}
	${IMGUI_INCLUDE_DIRS}
)

# compile librarie
add_library(ImGui
	"../include/imgui/imgui.cpp"				  # imgui
	"../include/imgui/imgui_demo.cpp"
	"../include/imgui/imgui_draw.cpp"
	"../include/imgui/imgui_tables.cpp"
	"../include/imgui/imgui_widgets.cpp"
	"../include/imgui/backends/imgui_impl_vulkan.cpp"
	"../include/imgui/backends/imgui_impl_sdl.cpp"
)

target_link_libraries(ImGui ${SDL2_LIBRARIES} Vulkan::Vulkan)

target_precompile_headers(ImGui
PRIVATE
	<vector>
	<iostream>
	<fstream>
	<deque>
	<string>
	<map>
	<unordered_map>
	<array>
	<math.h>
	<algorithm>
	<utility>
	<stdint.h>
	<functional>
	<vulkan/vulkan.h>
	<SDL.h>
	<SDL_error.h>
	<cmath>
)
