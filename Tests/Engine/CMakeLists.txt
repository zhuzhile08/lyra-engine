cmake_minimum_required(VERSION 3.24.0)
project(Engine VERSION 0.5.0)

# set default to debug
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_STANDARD 23)

find_package(Vulkan REQUIRED)

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
	${LIBRARY_PATH}/tol/
	${LIBRARY_PATH}/lz4/lib/
	${LIBRARY_PATH}/nlohmann/include/
	${LIBRARY_PATH}/vma/include/
)

# build the content manager
add_executable(Engine
	"src/main.cpp"
)

target_link_libraries(Engine
PRIVATE
	LyraEngine
)