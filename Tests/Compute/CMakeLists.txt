cmake_minimum_required(VERSION 3.24.0)

project(Compute VERSION 0.5.0)

include_directories(
PRIVATE
	# library local include directory
	${LYRA_INCLUDE_DIR}

	# graphics and windowing libraries
	Vulkan::Headers
	${LIBRARY_PATH}/sdl/include/

	# math and physics libraries
	${LIBRARY_PATH}/glm/

	# utility libraries
	${LIBRARY_PATH}/lsd/
	${LIBRARY_PATH}/stb/
	${LIBRARY_PATH}/assimp/include
	${LIBRARY_PATH}/lz4/lib/
	${LIBRARY_PATH}/vma/include/
	${LIBRARY_PATH}/fmt/include/
)

add_executable(Compute
	"src/main.cpp"
)

target_link_libraries(Compute
PRIVATE
	LyraEngine
)
