if (APPLE)
    set(VULKAN_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/include/")

    set(VULKAN_LIBRARIES "${CMAKE_SOURCE_DIR}/lib/libvulkan.1.3.224.dylib")
endif()