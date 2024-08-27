cmake_minimum_required (VERSION 3.24.0)

project(BuildLibraries)

set(ETCS_ENABLE_COMPONENTS_EXT ON CACHE BOOL "Use the already implemented components as an extension" FORCE)

# add all libraries that need to be built
# add_subdirectory("openal-soft")
if (BUILD_STATIC)
	set(SDL_SHARED OFF CACHE BOOL "Build a shared version of the library" FORCE)
	set(SDL_STATIC ON CACHE BOOL "Build a static version of the library" FORCE)
	set(LZ4_BUNDLED_MODE ON CACHE BOOL "Set LZ4 as bundled in the project" FORCE)
	set(IMGUI_BUILD_STATIC ON CACHE BOOL "Build ImGUI statically" FORCE)
	set(ETCS_STATIC ON CACHE BOOL "Build ETCS statically" FORCE)
else ()
	set(SDL_SHARED ON CACHE BOOL "Build a shared version of the library" FORCE)
	set(SDL_STATIC OFF CACHE BOOL "Build a static version of the library" FORCE)
	set(LZ4_BUNDLED_MODE ON CACHE BOOL "Set LZ4 as bundled in the project" FORCE)
	set(IMGUI_BUILD_STATIC OFF CACHE BOOL "Build ImGUI statically" FORCE)
	set(ETCS_STATIC OFF CACHE BOOL "Build ETCS statically" FORCE)
endif ()

add_subdirectory("lsd")
add_subdirectory("glm")
add_subdirectory("sdl")
add_subdirectory("lz4/build/cmake")
add_subdirectory("imgui")
add_subdirectory("fmt")
add_subdirectory("etcs")

# GLM and VMA aren't build here because they only build the tests, 
# which is not necceceary for the library to function 
