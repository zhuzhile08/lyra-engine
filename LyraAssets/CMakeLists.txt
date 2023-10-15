cmake_minimum_required(VERSION 3.24.0)

project(LyraAssets VERSION 1.0.0)

# find vulkan 
find_package(Vulkan REQUIRED)

# includes and stuff
include_directories(
PUBLIC
	# library local include directory
	${LYRA_INCLUDE_DIR}

	# graphics and windowing libraries
	Vulkan::Headers
	${LIBRARY_PATH}/sdl/include/
	${LIBRARY_PATH}/imgui/

	# math and physics libraries
	${LIBRARY_PATH}/glm/
	${LIBRARY_PATH}/bullet/src/

	# audio libraries
	${LIBRARY_PATH}/openal-soft/include/

	# utility libraries
	${LIBRARY_PATH}/stb/
	${LIBRARY_PATH}/assimp/include
	${LIBRARY_PATH}/pfd/
	${LIBRARY_PATH}/lz4/lib/
	${LIBRARY_PATH}/vma/include/
	${LIBRARY_PATH}/fmt/include/
)

# build the content manager
add_executable(LyraAssets
	"src/main.cpp"
	"src/SDLImGuiRenderer.cpp"
	"src/ContentManager.cpp"
	"src/GuiElements.cpp"
)

target_link_libraries(LyraAssets
PRIVATE
	LyraEngine

	assimp
)
