cmake_minimum_required(VERSION 3.24.0)
project(LyraEngine VERSION 0.5.0)

# debug build
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_STANDARD 23)

# compile shader commands
if (WIN32) 
	set(SHADER_COMPILE_COMMANDS_DIR "${CMAKE_SOURCE_DIR}/compileShader.bat")
else() 
	set(SHADER_COMPILE_COMMANDS_DIR "${CMAKE_SOURCE_DIR}/compileShader.sh")
endif()

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
	${LIBRARY_PATH}/assimp/include
	${LIBRARY_PATH}/lz4/lib/
	${LIBRARY_PATH}/vma/include/
	${LIBRARY_PATH}/fmt/include/
)

# compile shaders to Vulkan's spirv format
add_custom_target(build_shaders ALL
	COMMAND ${SHADER_COMPILE_COMMANDS_DIR}
	WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

# building the library
add_library(LyraEngine
	"src/Common/Logger.cpp"
	"src/Common/Benchmark.cpp"
	"src/Common/FileSystem.cpp"

	#"src/Graphics/VulkanImpl/CommandBuffer.cpp"
	#"src/Graphics/VulkanImpl/DescriptorSystem.cpp"
	#"src/Graphics/VulkanImpl/Device.cpp"
	#"src/Graphics/VulkanImpl/GPUBuffer.cpp"
	#"src/Graphics/VulkanImpl/Image.cpp"
	#"src/Graphics/VulkanImpl/PipelineBase.cpp"
	#"src/Resource/Shader.cpp"
	#"src/Graphics/VulkanImpl/Window.cpp"
	#"src/Graphics/VulkanImpl/Frame.cpp"

	"src/Graphics/VulkanRenderSystem.cpp"
	"src/Graphics/SDLWindow.cpp"
	#"src/Resource/Material.cpp"
	#"src/Resource/Texture.cpp"
	
	#"src/Application/Application.cpp"

	"src/Init/SingleHeaderInit.cpp"

	#"src/Math/LyraMath.cpp"
	#"src/Math/ComputePipeline.cpp"

	#"src/Resource/LoadResources.cpp"
	"src/Resource/LoadMesh.cpp"
	"src/Resource/LoadMaterial.cpp"
	"src/Resource/LoadTexture.cpp"
	#"src/Resource/ResourceManager.cpp"

	#"src/EntitySystem/Entity.cpp"
	#"src/EntitySystem/Camera.cpp"
	#"src/EntitySystem/Cubemap.cpp"
	#"src/EntitySystem/MeshFramebuffers.cpp"

	"src/Input/Input.cpp"
)

add_dependencies(LyraEngine build_shaders)

# link some libraries
target_link_libraries(LyraEngine
LINK_PUBLIC
	# graphics and windowing libraries
	Vulkan::Vulkan
	SDL2::SDL2
	SDL2::SDL2main
	ImGui

	# math and physics libraries
	# bullet holy crap is bullet a b to link. doing it when i actually need it

	# audio libraries
	# OpenAL::OpenAL

	# utility libraries
	assimp
	fmt::fmt
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
