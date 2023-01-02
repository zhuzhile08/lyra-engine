# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.24.0)

if (APPLE)
	set( CMAKE_CXX_COMPILER "/usr/bin/clang++" )
	set( CMAKE_C_COMPILER "/usr/bin/clang" )
endif()

project ("LyraEngineToolchain")

# set some variables
set(LIBRARY_PATH ${CMAKE_SOURCE_DIR}/3rdParty)

# Include sub-projects.
add_subdirectory ("3rdParty")
add_subdirectory ("LyraEngine")
# add_subdirectory ("LyraGui")
