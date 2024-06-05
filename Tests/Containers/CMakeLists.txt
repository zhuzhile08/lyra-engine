cmake_minimum_required(VERSION 3.24.0)

project(Containers VERSION 0.5.0)

include_directories(
PRIVATE
	# library local include directory
	${LYRA_INCLUDE_DIR}

	# graphics and windowing libraries
	Vulkan::Headers

	# math and physics libraries
	${LIBRARY_PATH}/glm/

	# utility libraries
	${LIBRARY_PATH}/lsd/
	${LIBRARY_PATH}/fmt/include
	${LIBRARY_PATH}/vma/include/
)

add_executable(Containers
	"src/main.cpp"
)

target_link_libraries(Containers
PRIVATE
	LyraEngine
)
