cmake_minimum_required(VERSION 3.22.0)
project(ContentManager VERSION 0.5.0)

# set default to debug
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_STANDARD 17)

# find and link the libraries
set(LYRA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake/)
set(SDL2_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake/)

find_package(LYRA REQUIRED)
find_package(SDL2 REQUIRED)

include_directories(
 ${SDL2_INCLUDE_DIRS} 
 ${LYRA_INCLUDE_DIRS} 
)

# build the content manager
add_executable(
 ""
)
