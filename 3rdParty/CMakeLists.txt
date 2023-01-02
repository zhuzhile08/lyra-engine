cmake_minimum_required (VERSION 3.24.0)

project(BuildLibraries)

set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_STANDARD 20)

# add all libraries that need to be built
add_subdirectory("bullet")
add_subdirectory("imgui")
add_subdirectory("lz4")
add_subdirectory("openal-soft")
add_subdirectory("sdl2")

# GLM and VMA aren't build here because they only build the tests, 
# which is not necceceary for the library to function 