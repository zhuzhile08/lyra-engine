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
#include <Common/Array.h>

#include <string_view>

namespace lyra {

namespace config {

// Disable logging enum
// Setting a logging level as config::disableLog only disables all severities BELOW it. This means that exceptions can't (and also shouldn't) be disabled
enum class DisableLog {
	none,
	trace,
	debug,
	info,
	warning,
	error,
	exception
};

inline constexpr DisableLog disableLog = DisableLog::none;
inline constexpr bool coloredLog = true;
inline constexpr bool displayFPS = false; // @todo

inline constexpr Array<const char*, 2> requestedDeviceExtensions({
	"VK_EXT_descriptor_indexing",
	"VK_KHR_swapchain"
});
inline constexpr Array<const char*, 1> requestedValidationLayers({ "VK_LAYER_KHRONOS_validation" });
inline constexpr size_type maxFramesInFlight = 2;
inline constexpr size_type maxSwapchainImages = 8;
inline constexpr size_type maxConcurrentRenderers = 16;
inline constexpr size_type maxDescriptorPoolSets = 512;
inline constexpr size_type maxShaderSets = 4;
inline constexpr size_type descriptorPoolAllocCount = 16;
inline constexpr uint32 maxDynamicBindings = 64;
inline constexpr bool enableAnistropy = true;
inline constexpr float32 anistropyStrength = 1.0f;
inline constexpr float32 resolution = 100;

inline constexpr std::string_view title = "Lyra Engine";
inline constexpr std::string_view iconPath = "";
inline constexpr uint32 windowWidth = 1280; // window width and height
inline constexpr uint32 windowHeight = 720;
inline constexpr bool resizable = true;
inline constexpr bool maximized = true;
inline constexpr bool borderless = false;
inline constexpr bool fullscreen = false;
inline constexpr bool alwaysOnTop = false;
inline constexpr bool vSync = false;
// inline constexpr bool

} // namespace config

} // namespace lyra
