/*************************
 * @file vulkan_wrappers.cpp
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief includes all core definitions
 *
 * @date 2022-02-05
 *
 * @copyright Copyright (c) 2022
 *************************/

#include <lyra.h>

#include "core/logger.cpp"
#include "core/settings.cpp"

#include "core/application.cpp"

#include "rendering/vulkan/command_buffer.cpp"
#include "rendering/vulkan/descriptor.cpp"
#include "rendering/vulkan/devices.cpp"
#include "rendering/vulkan/GPU_buffer.cpp"
#ifndef NDEBUG
#include "rendering/vulkan/GPU_memory.cpp"
#endif
#include "rendering/vulkan/vulkan_image.cpp"
#include "rendering/vulkan/vulkan_pipeline.cpp"
#include "rendering/vulkan/vulkan_shader.cpp"
#include "rendering/vulkan/vulkan_window.cpp"

#include "rendering/assets.cpp"
#include "rendering/render_system.cpp"
#include "rendering/gui_renderer.cpp"
#include "rendering/graphics_pipeline.cpp"
#include "rendering/material.cpp"
#include "rendering/renderer.cpp"
#include "rendering/texture.cpp"
#include "rendering/window.cpp"

#include "init/init_SDL.cpp"
#include "init/single_header_init.cpp"

#include "math/math.cpp"
#include "math/transform.cpp"

#include "res/loaders.cpp"

#include "nodes/nodes.cpp" 

#include "input/input.cpp"

#include "core/benchmark.inl"
