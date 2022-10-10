set(SDL2_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/include/SDL2/")

if(WIN32)
	# Support both 32 and 64 bit builds
	if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
		set(SDL2_LIBRARIES "${CMAKE_SOURCE_DIR}/lib/x64/SDL2.lib;${CMAKE_SOURCE_DIR}/lib/x64/SDL2main.lib")
	else ()
		set(SDL2_LIBRARIES "${CMAKE_SOURCE_DIR}/lib/x86/SDL2.lib;${CMAKE_SOURCE_DIR}/lib/x86/SDL2main.lib")
	endif ()
elseif(APPLE)
	# set(SDL2_LIBRARIES "${CMAKE_SOURCE_DIR}/lib/libSDL2.a;${CMAKE_SOURCE_DIR}/lib/libSDL2main.a")
	set(SDL2_LIBRARIES "${CMAKE_SOURCE_DIR}/lib/libSDL2-2.0.dylib;${CMAKE_SOURCE_DIR}/lib/libSDL2main.a")
endif()
