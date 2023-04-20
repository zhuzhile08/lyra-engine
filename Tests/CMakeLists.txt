cmake_minimum_required (VERSION 3.24.0)

project(Tests)

set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_STANDARD 20)

# add all tests that need to be built
add_subdirectory("Containers")
add_subdirectory("Engine")
