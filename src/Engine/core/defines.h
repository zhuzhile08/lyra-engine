/*************************
 * @file defines.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief macros and settings
 * @brief mathematical definitions for numbers and fixed width integers
 * @brief miscelanious definitions
 * @brief engine and window settings
 * 
 * @date 2022-02-03
 * 
 * @copyright Copyright (c) 2022
 * 
 *************************/

#pragma once

#include <cstdint>
#include <chrono>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace lyra {

// some other definitions
typedef const char*         str;            // please don't look at this
#define ARR_SIZE(array)     sizeof(array)/sizeof(array[0])
#define GET_ADDRESS(var)    (void*)var

// define window settings
#define WIDTH 690
#define HEIGHT 420
#define FOV 60
#define TITLE "Game"
#define RESIZEABLE true
#define FULLSCREEN false

// some engine specific definitions
const std::vector<str>			requested_validation_layers{"VK_LAYER_KHRONOS_validation"};
const std::vector<str>			requested_device_extensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
#define ENABLE_MULTI_SAMPLING   0
#define ENABLE_COLOR_BLENDING   0
#define CLEAR_COLOR             0.0f, 0.0f, 0.0f, 1.0f
#define USING_GUI               0
#define DOUBLE_BUFFERING        0
#define MAX_FRAMES_IN_FLIGHT    2
#define POLYGON_MODE            VK_POLYGON_MODE_FILL

} // namespace lyra