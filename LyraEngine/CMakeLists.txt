cmake_minimum_required(VERSION 3.22.0)
project(LyraEngine VERSION 0.5.0)

# set default to debug
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_STANDARD 20)

# directories for the config files
set(SDL2_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(GLM_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(VMA_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(TOL_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(STB_DIR ${CMAKE_SOURCE_DIR}/cmake) # temporary
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(LZ4_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(JSON_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(ENGINE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/)

# find path to libraries
find_package(SDL2 REQUIRED)
find_package(GLM REQUIRED)
find_package(VMA REQUIRED)
find_package(STB REQUIRED) # temporary
find_package(TOL REQUIRED)
find_package(IMGUI REQUIRED)
find_package(LZ4 REQUIRED)
find_package(JSON REQUIRED)
find_package(Vulkan REQUIRED)

if (WIN32)
	# shaders
	add_custom_target(build_shaders ALL
		COMMAND "${CMAKE_SOURCE_DIR}/compileShader.bat"
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
	)

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
	add_library(LyraEngine
		"../include/imgui/imgui.cpp"				  # imgui
		"../include/imgui/imgui_demo.cpp"
		"../include/imgui/imgui_draw.cpp"
		"../include/imgui/imgui_tables.cpp"
		"../include/imgui/imgui_widgets.cpp"
		"../include/imgui/backends/imgui_impl_vulkan.cpp"
		"../include/imgui/backends/imgui_impl_sdl.cpp"
		"src/init/init_SDL.cpp"					# engine
		"src/init/single_header_init.cpp"
		"src/math/math.cpp"
		"src/res/loaders.cpp"
		"src/nodes/nodes.cpp" 
		"src/core/def.cpp"
	)
elseif (APPLE)
	# shaders
	add_custom_target(build_shaders ALL
		COMMAND "${CMAKE_SOURCE_DIR}/compileShader.sh"
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
	)

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

	# building the library
	add_library(LyraEngine
		"../include/imgui/imgui.cpp"				  # imgui
		"../include/imgui/imgui_demo.cpp"
		"../include/imgui/imgui_draw.cpp"
		"../include/imgui/imgui_tables.cpp"
		"../include/imgui/imgui_widgets.cpp"
		"../include/imgui/backends/imgui_impl_vulkan.cpp"
		"../include/imgui/backends/imgui_impl_sdl.cpp"
		"src/init/init_SDL.cpp"					# engine
		"src/init/single_header_init.cpp"
		"src/math/math.cpp"
		"src/res/loaders.cpp"
		"src/nodes/nodes.cpp" 
		"src/core/def.cpp"
	)
endif()

# add_dependencies(LyraEngine build_shaders)

if (WIN32)
	target_link_libraries(LyraEngine ${SDL2_LIBRARIES} ${LZ4_LIBRARIES} Vulkan::Vulkan)
elseif (APPLE)
	target_link_libraries(LyraEngine ${SDL2_LIBRARIES} ${LZ4_LIBRARIES} ${VULKAN_LIBRARIES})
endif()

target_precompile_headers(LyraEngine
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
	<backends/imgui_impl_sdl.h>
	<imgui.h>
	<backends/imgui_impl_vulkan.h>
	<lz4.h>
	<vulkan/vulkan.h>
	<SDL.h>
	<SDL_error.h>
	<glm.hpp>
	<cmath>
	<gtc/matrix_transform.hpp>
	<gtx/matrix_decompose.hpp>
	<nlohmann/json.hpp>
)
