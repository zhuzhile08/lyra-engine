cmake_minimum_required(VERSION 3.24.0)
project(LyraEngine VERSION 0.5.0)

# debug build
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_STANDARD 20)

# find vulkan 
find_package(Vulkan REQUIRED)

# includes and stuff
include_directories(
PUBLIC
	# library local include directory
	${LYRA_INCLUDE_DIR}

	# graphics and windowing libraries
	Vulkan::Headers
	${LIBRARY_PATH}/sdl2/include/
	${LIBRARY_PATH}/imgui/

	# math and physics libraries
	${LIBRARY_PATH}/glm/
	${LIBRARY_PATH}/bullet/src/

	# audio libraries
	${LIBRARY_PATH}/openal-soft/include/

	# utility libraries
	${LIBRARY_PATH}/stb/
	${LIBRARY_PATH}/tol/
	${LIBRARY_PATH}/lz4/lib/
	${LIBRARY_PATH}/nlohmann/include/
	${LIBRARY_PATH}/vma/include/
)

# compile shaders to Vulkan's spirv format
add_custom_target(build_shaders ALL
	COMMAND ${SHADER_COMPILE_COMMANDS_DIR}
	WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

# building the library
add_library(LyraEngine
	"src/core/logger.cpp"
	"src/core/settings.cpp"

	"src/core/application.cpp"

	"src/rendering/vulkan/command_buffer.cpp"
	"src/rendering/vulkan/descriptor.cpp"
	"src/rendering/vulkan/devices.cpp"
	"src/rendering/vulkan/GPU_buffer.cpp"
	"src/rendering/vulkan/vulkan_image.cpp"
	"src/rendering/vulkan/vulkan_pipeline.cpp"
	"src/rendering/vulkan/vulkan_shader.cpp"
	"src/rendering/vulkan/vulkan_window.cpp"

	"src/rendering/assets.cpp"
	"src/rendering/render_system.cpp"
	"src/rendering/gui_renderer.cpp"
	"src/rendering/graphics_pipeline.cpp"
	"src/rendering/material.cpp"
	"src/rendering/renderer.cpp"
	"src/rendering/texture.cpp"
	"src/rendering/window.cpp"

	"src/init/init_SDL.cpp"
	"src/init/single_header_init.cpp"

	"src/math/math.cpp"
	"src/math/transform.cpp"

	"src/res/loaders/load_assets.cpp"
	"src/res/loaders/load_file.cpp"
	"src/res/loaders/load_model.cpp"

	"src/nodes/graphics/camera.cpp"
	# "src/nodes/graphics/cubemap.cpp"

	"src/nodes/mesh/mesh.cpp"
	"src/nodes/mesh/mesh_renderer.cpp"

	"src/input/input.cpp"

	"src/core/benchmark.inl"
 	"src/rendering/vulkan/frame.cpp"
)

add_dependencies(LyraEngine build_shaders)

# link some libraries
target_link_libraries(LyraEngine
LINK_PUBLIC
	# graphics and windowing libraries
	Vulkan::Vulkan
	SDL2::SDL2-static
	SDL2::SDL2main
	ImGui

	# math and physics libraries
	# bullet holy crap is bullet a b to link. doing it when i actually need it

	# audio libraries
	# OpenAL::OpenAL

	# utility libraries
	lz4_static
)

target_precompile_headers(LyraEngine
PUBLIC
	<vector>
	<iostream>
	<fstream>
	<deque>
	<string>
	<map>
	<unordered_map>
	<array>
	<algorithm>
	<chrono>
	<utility>
	<filesystem>
	<stddef.h>
	<stdint.h>
	<wchar.h>
	<functional>
	<cmath>
)
