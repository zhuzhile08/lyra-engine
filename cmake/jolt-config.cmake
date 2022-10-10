if(WIN32) # since the Jolt include dir will just be include itself, I won't put an extra JOLT_INCLUDE_DIRS here
	# Support both 32 and 64 bit builds
	if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
		set(JOLT_LIBRARIES "${CMAKE_SOURCE_DIR}/lib/x64/JOLT.lib;${CMAKE_SOURCE_DIR}/lib/x64/JOLTmain.lib")
	else ()
		set(JOLT_LIBRARIES "${CMAKE_SOURCE_DIR}/lib/x86/JOLT.lib;${CMAKE_SOURCE_DIR}/lib/x86/JOLTmain.lib")
	endif ()
elseif(APPLE)
	set(JOLT_LIBRARIES "${CMAKE_SOURCE_DIR}/lib/libJolt.a")
endif()