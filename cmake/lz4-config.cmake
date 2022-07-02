set(LZ4_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/include/LZ4/")

# Support both 32 and 64 bit builds
if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
  set(LZ4_LIBRARIES "${CMAKE_SOURCE_DIR}/lib/x64/liblz4.lib")
else ()
  set(LZ4_LIBRARIES "${CMAKE_SOURCE_DIR}/lib/x86/liblz4.lib")
endif ()

string(STRIP "${LZ4_LIBRARIES}" LZ4_LIBRARIES)
