# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.24.0)
set(CMAKE_OSX_DEPLOYMENT_TARGET "14.3" CACHE STRING "Minimum OS X deployment version")

project (LyraEngineToolchain)


# project options
option(BUILD_STATIC "Build LyraEngine statically" ON)
if (BUILD_STATIC)
	option(BUILD_SHARED_LIBS "Build all 3rd party libraries and LyraEngine as shared" OFF)
else ()
	option(BUILD_SHARED_LIBS "Build all 3rd party libraries and LyraEngine as shared" ON)
endif()


# apple compiler path
if (APPLE)
	set( CMAKE_CXX_COMPILER "/usr/bin/clang++" )
	set( CMAKE_C_COMPILER "/usr/bin/clang" )
endif()


# custom validation layer options
option(USE_CUSTOM_VULKAN_VALIDATION_LAYERS "Use vulkan validation layers from a custom path instead of the default one" OFF)


# 3rd party library path
set(LIBRARY_PATH ${CMAKE_SOURCE_DIR}/3rdParty)


# set the local include directory for the engine
set(LYRA_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/LyraEngine/include/)

# copy data to the binary folder
file(COPY ${CMAKE_SOURCE_DIR}/data DESTINATION ${CMAKE_BINARY_DIR}/Tests/Engine)
file(COPY ${CMAKE_SOURCE_DIR}/LyraAssets/data DESTINATION ${CMAKE_BINARY_DIR}/LyraAssets)


# set global options
set(CMAKE_CXX_STANDARD 23)

# Include sub-projects.
add_subdirectory ("3rdParty")
add_subdirectory ("LyraEngine")
add_subdirectory ("LyraAssets")
add_subdirectory ("Tests")
