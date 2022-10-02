# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

if (APPLE)
    set( CMAKE_CXX_COMPILER "/usr/bin/g++" )
    set( CMAKE_C_COMPILER "/usr/bin/gcc" )
endif()

project ("LyraEngineToolchain")

# Include sub-projects.
add_subdirectory ("ImGuiBuild")
add_subdirectory ("LyraEngine")
add_subdirectory ("LyraGui")
