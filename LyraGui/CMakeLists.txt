cmake_minimum_required(VERSION 3.25.0)
project(LyraGui VERSION 0.5.0)

# set default to debug
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_STANDARD 23)

set(lyra_DIR ${CMAKE_SOURCE_DIR}/LyraEngine)
find_package(lyra REQUIRED)

find_package(Vulkan REQUIRED)

include_directories(
PRIVATE
	${LYRA_INCLUDE_DIRS}
	"/opt/homebrew/include/"
)

# build the content manager
add_executable(LyraGui
	"src/main.cpp"
)

target_link_libraries(LyraGui
PRIVATE
	${LYRA_LIBRARIES}
)
