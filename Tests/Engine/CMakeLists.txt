cmake_minimum_required(VERSION 3.24.0)

project(Engine VERSION 0.5.0)

include_directories(
PRIVATE
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

add_executable(Engine
	"src/main.cpp"
)

target_link_libraries(Engine
PRIVATE
	LyraEngine
)
