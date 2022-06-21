cmake_minimum_required(VERSION 3.22.0)
project(ContentManager VERSION 0.5.0)

# set default to debug
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_STANDARD 17)

# find and link the libraries
set(LYRA_DIR ${CMAKE_SOURCE_DIR}/cmake/)
set(SDL2_DIR ${CMAKE_SOURCE_DIR}/cmake/)
set(NOUD_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(GLM_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(STB_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(VMA_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(TOL_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/cmake)

find_package(LYRA REQUIRED)
find_package(SDL2 REQUIRED)
find_package(NOUD REQUIRED)
find_package(GLM REQUIRED)
find_package(STB REQUIRED)
find_package(VMA REQUIRED)
find_package(TOL REQUIRED)
find_package(IMGUI REQUIRED)
find_package(Vulkan REQUIRED)

include_directories(
 ${SDL2_INCLUDE_DIRS} 
 ${LYRA_INCLUDE_DIRS}
 ${NOUD_INCLUDE_DIRS} 
 ${GLM_INCLUDE_DIRS} 
 ${Vulkan_INCLUDE_DIRS} 
 ${GUI_INCLUDE_DIR}
 ${STB_INCLUDE_DIRS} 
 ${VMA_INCLUDE_DIRS} 
 ${TOL_INCLUDE_DIRS} 
 ${IMGUI_INCLUDE_DIRS}
)

# build the content manager
add_executable(ContentManager
 "src/main.cpp"
)

target_link_libraries(ContentManager ${LYRA_LIBRARIES} ${SDL2_LIBRARIES} Vulkan::Vulkan)
