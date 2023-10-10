cmake_minimum_required (VERSION 3.24.0)

project(BuildLibraries)

# add all libraries that need to be built
# add_subdirectory("openal-soft")
add_subdirectory("sdl2")
add_subdirectory("lz4/build/cmake")
add_subdirectory("imgui")
add_subdirectory("fmt")
add_subdirectory("assimp")

# GLM and VMA aren't build here because they only build the tests, 
# which is not necceceary for the library to function 
