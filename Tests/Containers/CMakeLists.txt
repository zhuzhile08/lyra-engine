cmake_minimum_required(VERSION 3.24.0)
project(Containers VERSION 0.5.0)

# set default to debug
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_STANDARD 23)

include_directories(
PRIVATE
	# library local include directory
	${LYRA_INCLUDE_DIR}

	# graphics and windowing libraries
	Vulkan::Headers

	# math and physics libraries
	${LIBRARY_PATH}/glm/

	# utility libraries
	${LIBRARY_PATH}/fmt/
	${LIBRARY_PATH}/vma/include/
)

add_executable(Containers
	"src/main.cpp"
)

target_link_libraries(Containers
PRIVATE
	LyraEngine
)
