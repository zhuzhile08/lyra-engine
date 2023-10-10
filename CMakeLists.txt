# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.24.0)

project (LyraEngineToolchain)


# project options
option(BUILD_STATIC "Build LyraEngine statically" TRUE)
if (BUILD_STATIC)
	set (BUILD_SHARED_LIBS FALSE)
else ()
	set (BUILD_SHARED_LIBS TRUE)
endif ()

option(BUILD_DEBUG "Build the toolchain in debug mode" TRUE)
if (BUILD_DEBUG)
	set (BUILD_MODE DEBUG)
else ()
	set (BUILD_MODE RELEASE)
endif ()


# apple compiler path
if (APPLE)
	set( CMAKE_CXX_COMPILER "/usr/bin/clang++" )
	set( CMAKE_C_COMPILER "/usr/bin/clang" )
endif()


# set some variables
set(LIBRARY_PATH ${CMAKE_SOURCE_DIR}/3rdParty)


# set the local include directory for the engine
set(LYRA_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/LyraEngine/include/)


# set global options
set(CMAKE_BUILD_TYPE BUILD_MODE)
set(CMAKE_CXX_STANDARD 23)

# Include sub-projects.
add_subdirectory ("3rdParty")
add_subdirectory ("LyraEngine")
add_subdirectory ("LyraAssets")
add_subdirectory ("Tests")
