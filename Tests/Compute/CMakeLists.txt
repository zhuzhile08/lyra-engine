cmake_minimum_required(VERSION 3.24.0)
project(Compute VERSION 0.5.0)

# set default to debug
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_STANDARD 23)

include_directories(
PRIVATE
	# library local include directory
	${LYRA_INCLUDE_DIR}

	# graphics and windowing libraries
	Vulkan::Headers
	${LIBRARY_PATH}/sdl2/include/

	# math and physics libraries
	${LIBRARY_PATH}/glm/

	# utility libraries
	${LIBRARY_PATH}/stb/
	${LIBRARY_PATH}/tol/
	${LIBRARY_PATH}/lz4/lib/
	${LIBRARY_PATH}/vma/include/
	${LIBRARY_PATH}/fmt/
)

add_executable(Compute
	"src/main.cpp"
)

target_link_libraries(Compute
PRIVATE
	LyraEngine
)
