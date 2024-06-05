cmake_minimum_required(VERSION 3.24.0)

project(Engine VERSION 0.5.0)

include_directories(
PRIVATE
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
	${LIBRARY_PATH}/lz4/lib/
	${LIBRARY_PATH}/vma/include/
	${LIBRARY_PATH}/fmt/include/
)

# copy data files to the build location
add_custom_target(copy_engine_data
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/data/
)

add_executable(Engine
	"src/main.cpp"
)

add_dependencies(Engine copy_engine_data)

target_link_libraries(Engine
PRIVATE
	LyraEngine
)
