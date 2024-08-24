cmake_minimum_required(VERSION 3.24.0)

project(LyraEngine VERSION 0.5.0)

# compile shader commands
if (WIN32) 
	set(SHADER_COMPILE_COMMANDS_DIR ${CMAKE_SOURCE_DIR}/compileShader.bat)
else () 
	set(SHADER_COMPILE_COMMANDS_DIR ${CMAKE_SOURCE_DIR}/compileShader.sh)
endif ()

# find vulkan 
find_package(Vulkan REQUIRED)

# includes and stuff
include_directories(
PUBLIC
	# library local include directory
	${LYRA_INCLUDE_DIR}

	# graphics and windowing libraries
	Vulkan::Headers
	${LIBRARY_PATH}/sdl/include/
	${LIBRARY_PATH}/imgui/

	# math and physics libraries
	${LIBRARY_PATH}/glm/
	${LIBRARY_PATH}/bullet/src/

	# audio libraries
	${LIBRARY_PATH}/openal-soft/include/

	# utility libraries
	${LIBRARY_PATH}/stb/
	${LIBRARY_PATH}/lz4/lib/
	${LIBRARY_PATH}/vma/include/
	${LIBRARY_PATH}/fmt/include/
)

# compile shaders to Vulkan's spirv format
add_custom_target(BuildShaders ALL
	COMMAND ${SHADER_COMPILE_COMMANDS_DIR}
	WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

set (LYRA_ENGINE_SOURCE_FILES 
	"src/Common/Logger.cpp"
	"src/Common/Benchmark.cpp"
	"src/Common/FileSystem.cpp"

	"src/Graphics/VulkanRenderSystem.cpp"
	"src/Graphics/Window.cpp"
	"src/Graphics/ImGuiRenderer.cpp"
	"src/Graphics/Renderer.cpp"
	"src/Graphics/Material.cpp"
	"src/Graphics/Texture.cpp"
	
	#"src/Application/Application.cpp"

	"src/Init/SingleHeaderInit.cpp"

	#"src/Math/LyraMath.cpp"
	#"src/Math/ComputePipeline.cpp"

	#"src/Resource/LoadResources.cpp"
	"src/Resource/LoadMeshFile.cpp"
	#"src/Resource/LoadMaterial.cpp"
	"src/Resource/LoadTextureFile.cpp"
	"src/Resource/ResourceSystem.cpp"

	"src/Components/Camera.cpp"
	#"src/Components/Cubemap.cpp"
	"src/Components/MeshRenderer.cpp"

	"src/Input/InputSystem.cpp"
)

# building the library
if (BUILD_STATIC)
	add_library(LyraEngine STATIC "${LYRA_ENGINE_SOURCE_FILES}")
else ()
	add_library(LyraEngine SHARED "${LYRA_ENGINE_SOURCE_FILES}")
endif ()

# build error level
if (WIN32) 
	target_compile_options(LyraEngine PRIVATE /WX)
else () 
	target_compile_options(LyraEngine PRIVATE -Wall -Wextra -Wpedantic)
endif ()


# custom validation layer
if (USE_CUSTOM_VULKAN_VALIDATION_LAYERS)
	include("include/ValidationLayerPath.cmake")
	target_compile_definitions(LyraEngine PRIVATE CUSTOM_VULKAN_VALIDATION_LAYER)
	target_compile_definitions(LyraEngine PRIVATE CUSTOM_VULKAN_VALIDATION_LAYER_PATH="${VALIDATION_LAYER_PATH}")
endif ()


add_dependencies(LyraEngine BuildShaders)

# link some libraries
target_link_libraries(LyraEngine
LINK_PUBLIC
	# graphics and windowing libraries
	Vulkan::Vulkan
	SDL3::SDL3-static
	ImGui

	# math and physics libraries
	# bullet holy crap is bullet a b to link. doing it when i actually need it

	# audio libraries
	# OpenAL::OpenAL

	# utility libraries
	LyraStandardLibrary::Headers
	ETCS::Headers
	ETCS::ETCS-static
	fmt::fmt
	lz4_static
)

target_precompile_headers(LyraEngine
PUBLIC
	<typeindex>
	<type_traits>
	<algorithm>
	<chrono>
	<cmath>
	<cstdio>
	<cstdint>
	<cstdlib>
	<deque>
	<filesystem>
	<fstream>
	<functional>
	<iostream>
	<initializer_list>
	<map>
	<set>
	<stddef.h>
	<stdexcept>
	<stdint.h>
	<type_traits>
	<utility>
	<wchar.h>
)
