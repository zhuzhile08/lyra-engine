#define VMA_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#ifdef __APPLE__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-extension"
#pragma clang diagnostic ignored "-Wnullability-completeness"
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#include <vk_mem_alloc.h>
#pragma clang diagnostic pop
#else
#include <vk_mem_alloc.h>
#endif

#include <stb_image.h>
