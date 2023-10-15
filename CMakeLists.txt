# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.24.0)

project (LyraEngineToolchain)


# project options
option(BUILD_STATIC "Build LyraEngine statically" ON)
if (BUILD_STATIC)
	option(BUILD_SHARED_LIBS "Build all 3rd party libraries and LyraEngine as shared" OFF)
else ()
	option(BUILD_SHARED_LIBS "Build all 3rd party libraries and LyraEngine as shared" ON)
endif()

option(BUILD_DEBUG "Build the toolchain in debug mode" ON)
if (BUILD_DEBUG)
	set(CMAKE_BUILD_TYPE Debug)
else ()
	set(CMAKE_BUILD_TYPE Release)
endif ()


# apple compiler path
if (APPLE)
	set( CMAKE_CXX_COMPILER "/usr/bin/clang++" )
	set( CMAKE_C_COMPILER "/usr/bin/clang" )
endif()


# 3rd party library path
set(LIBRARY_PATH ${CMAKE_SOURCE_DIR}/3rdParty)


# set the local include directory for the engine
set(LYRA_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/LyraEngine/include/)


# set global options
set(CMAKE_CXX_STANDARD 23)

# Include sub-projects.
add_subdirectory ("3rdParty")
add_subdirectory ("LyraEngine")
add_subdirectory ("LyraAssets")
add_subdirectory ("Tests")
