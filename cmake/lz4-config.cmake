set(LZ4_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/include/lz4/")

if (WIN32)
	# Support both 32 and 64 bit builds
	if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
	set(LZ4_LIBRARIES "${CMAKE_SOURCE_DIR}/lib/x64/liblz4.lib")
	else ()
	set(LZ4_LIBRARIES "${CMAKE_SOURCE_DIR}/lib/x86/liblz4.lib")
	endif ()
elseif (APPLE)
	set(LZ4_LIBRARIES "${CMAKE_SOURCE_DIR}/lib/liblz4.a")
endif()
