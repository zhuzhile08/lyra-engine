cmake_minimum_required(VERSION 3.24.2)
project(LyraGui VERSION 0.5.0)

# set default to debug
set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CXX_STANDARD 20)

# find and link the libraries
set(LYRA_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(SDL2_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(GLM_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(STB_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(VMA_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(TOL_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(LZ4_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(JSON_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(JOLT_DIR ${CMAKE_SOURCE_DIR}/cmake)
set(MANAGER_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src)

find_package(LYRA REQUIRED)
find_package(SDL2 REQUIRED)
find_package(GLM REQUIRED)
find_package(STB REQUIRED)
find_package(VMA REQUIRED)
find_package(TOL REQUIRED)
find_package(IMGUI REQUIRED)
find_package(LZ4 REQUIRED)
find_package(JSON REQUIRED)
find_package(JOLT REQUIRED)
find_package(Vulkan REQUIRED)

include_directories(
	${SDL2_INCLUDE_DIRS} 
	${LYRA_INCLUDE_DIRS}
	${GLM_INCLUDE_DIRS} 
	${Vulkan_INCLUDE_DIRS} 
	${ENGINE_INCLUDE_DIR}
	${GUI_INCLUDE_DIR}
	${STB_INCLUDE_DIRS}
	${VMA_INCLUDE_DIRS} 
	${TOL_INCLUDE_DIRS} 
	${JSON_INCLUDE_DIRS}
	${LZ4_INCLUDE_DIRS}
	${IMGUI_INCLUDE_DIRS}
	${MANAGER_INCLUDE_DIRS}
)

# build the content manager
add_executable(LyraGui
	"src/main.cpp"
)

target_link_libraries(LyraGui ${SDL2_LIBRARIES} ${JOLT_LIBRARIES} ${LZ4_LIBRARIES} ${IMGUI_LIBRARIES} ${LYRA_LIBRARIES} Vulkan::Vulkan)
