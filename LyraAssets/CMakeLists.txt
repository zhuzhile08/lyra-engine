cmake_minimum_required(VERSION 3.24.0)

project(LyraAssets VERSION 1.0.0)

# find vulkan 
find_package(Vulkan REQUIRED)

target_compile_definitions(LyraEngine PUBLIC LSD_ENUM_UTILITIES)

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
	${LIBRARY_PATH}/lsd/
	${LIBRARY_PATH}/stb/
	${LIBRARY_PATH}/tinygltf/
	${LIBRARY_PATH}/pfd/
	${LIBRARY_PATH}/lz4/lib/
	${LIBRARY_PATH}/vma/include/
	${LIBRARY_PATH}/fmt/include/
)

# copy data files to the build location
add_custom_target(copy_assets_data
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/LyraAssets/data ${CMAKE_BINARY_DIR}/data/
)

# build the content manager
add_executable(LyraAssets
	"src/main.cpp"
	"src/ContentManager.cpp"
	"src/GuiElements.cpp"
)

# build error level
if (WIN32) 
	target_compile_options(LyraAssets PRIVATE /WX)
else () 
	target_compile_options(LyraAssets PRIVATE -Wall -Wextra -Wpedantic)
endif ()

add_dependencies(LyraAssets copy_assets_data)

target_link_libraries(LyraAssets
PRIVATE
	LyraEngine
)
