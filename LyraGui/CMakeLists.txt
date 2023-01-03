cmake_minimum_required(VERSION 3.24.0)
project(LyraGui VERSION 0.5.0)

# set default to debug
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_STANDARD 23)

find_package(Vulkan REQUIRED)

include_directories(
PRIVATE
	${LYRA_INCLUDE_DIR}
)

# build the content manager
add_executable(LyraGui
	"src/main.cpp"
)

target_link_libraries(LyraGui
PRIVATE
	LyraEngine
)
