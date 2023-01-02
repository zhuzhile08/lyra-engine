cmake_minimum_required(VERSION 3.24.0)
project(LyraGui VERSION 1.0.0)

# set default to debug
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_STANDARD 23)

# build the content manager
add_executable(LyraAssets
 "src/main.cpp"
 "src/logger.cpp")
