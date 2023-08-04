/*************************
 * @file Config.h
 * @author zhuzhile08 (zhuzhile08@gmail.com)
 * 
 * @brief a file containing engine and application settings
 * 
 * @version 0.1
 * @date 2022-12-26
 * 
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <Common/Common.h>
#include <Common/Dynarray,h>

namespace lyra {

namespace config {

// disable logging enum
enum class DisableLog {
	none = 0x00000001,
	debug = 0x00000002,
	info = 0x00000004,
	warning = 0x00000008,
	error = 0x0000001,
	exception = 0x00000020,
	all = 0x00000040
};

inline constexpr DisableLog disableLog = DisableLog::none;
inline constexpr bool displayFPS = false; // @todo
inline constexpr Dynarray<const char*, 10> requestedDeviceExtensions = {
	"VK_KHR_swapchain", 
	"VK_KHR_portability_subset", 
	"VK_EXT_descriptor_indexing", 
	"VK_KHR_swapchain"
#ifdef _WIN32
};
#elif __APPLE__
	, "VK_KHR_portability_subset" 
};
#endif
inline constexpr Dynarray<const char*, 10> requestedValidationLayers = { "VK_LAYER_KHRONOS_validation" };
inline constexpr size_t maxFramesInFlight = 2;
inline constexpr size_t maxSwapchainImages = 8;
inline constexpr size_t maxConcurrentRenderers = 16;
inline constexpr bool anistropy = true;
inline constexpr float32 anistropyStrength = 1.0f;
inline constexpr float32 resolution = 100;
inline constexpr const char* title = "Lyra Engine";
inline constexpr const char* iconPath = "";
inline constexpr uint32 windowWidth = 1280; // window width and height
inline constexpr uint32 windowHeight = 720;
inline constexpr bool resizable = true;
inline constexpr bool maximized = true;
inline constexpr bool borderless = false;
inline constexpr bool fullscreen = false;
inline constexpr bool alwaysOnTop = false;
inline constexpr bool vSync = false;
inline constexpr size_t maxDescriptorSetLayouts = 4;
inline constexpr size_t maxDescriptorTypePerPool = 8;

} // namespace config

} // namespace lyra
