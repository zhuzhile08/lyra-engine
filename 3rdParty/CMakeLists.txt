cmake_minimum_required (VERSION 3.24.0)

project(BuildLibraries)

# add all libraries that need to be built
# add_subdirectory("openal-soft")
if (BUILD_STATIC)
	option(SDL_SHARED "Build a shared version of the library" OFF)
	option(SDL_STATIC "Build a static version of the library" ON)
	option(LZ4_BUNDLED_MODE "Set LZ4 as bundled in the project" ON)
	option(IMGUI_BUILD_STATIC "Build ImGUI statically" ON)
else ()
	option(SDL_SHARED "Build a shared version of the library" ON)
	option(SDL_STATIC "Build a static version of the library" OFF)
	option(LZ4_BUNDLED_MODE "Set LZ4 as bundled in the project" ON)
	option(IMGUI_BUILD_STATIC "Build ImGUI statically" OFF)
endif ()

add_subdirectory("sdl")
add_subdirectory("lz4/build/cmake")
add_subdirectory("imgui")
add_subdirectory("fmt")
add_subdirectory("assimp")

# GLM and VMA aren't build here because they only build the tests, 
# which is not necceceary for the library to function 
