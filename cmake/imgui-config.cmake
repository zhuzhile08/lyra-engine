set(IMGUI_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/include/imgui/")

if (WIN32)
	# Support both 32 and 64 bit builds
	if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
		set(IMGUI_LIBRARIES "${CMAKE_SOURCE_DIR}/out/build/x64-debug/LyraEngine/LyraEngine.lib") # don't forget to change these to the release builds
	else ()
		set(IMGUI_LIBRARIES "${CMAKE_SOURCE_DIR}/out/build/x86-debug/LyraEngine/LyraEngine.lib") # also don't forget to change this to the x86 version
	endif ()
elseif(APPLE)
	set(IMGUI_LIBRARIES "${CMAKE_SOURCE_DIR}/out/build/macos-debug/ImGuiBuild/libImGui.a") # nor this
endif()

