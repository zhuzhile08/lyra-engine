cmake_minimum_required (VERSION 3.24.0)

project(BuildLibraries)

set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_STANDARD 23)

# add all libraries that need to be built
# add_subdirectory("openal-soft")
add_subdirectory("sdl2")
add_subdirectory("lz4/build/cmake")
add_subdirectory("imgui")
add_subdirectory("fmt")

if (WIN32) 
    set(CMAKE_CXX_STANDARD 14) # since the std::tr1 is deprecated from Visual Studio 17 onwards, I'll have to do this until the issue with gtest_printer.h is fixed. @todo
endif()

add_subdirectory("bullet3")

# GLM and VMA aren't build here because they only build the tests, 
# which is not necceceary for the library to function 
