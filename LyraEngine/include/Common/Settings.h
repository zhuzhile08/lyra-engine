/*************************
 * @file Settings.h
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
#include <Json/Json.h>

#include <vector>
#include <string>

namespace lyra {

class Settings {
public:
	// debug mode enum
	enum class DebugMode : int {
		debug = 1,
		release = 2,
	};

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

	// generell application settings
	struct AppConfig {
		std::string description;
		int fps;
	};

	// debug settings
	struct DebugConfig {
		DebugMode debug;
		DisableLog disableLog;
		bool printFPS;
		bool stdioSync;

		std::vector <const char*> requestedDeviceExtensions;
		std::vector <const char*> requestedValidationLayers;
	};

	// rendering settings
	struct RenderConfig {
		constexpr static size_t maxFramesInFlight = 2;
		constexpr static size_t maxSwapchainImages = 8;
		constexpr static size_t maxConcurrentRenderers = 16;

		float32 fov;

		bool anistropy;
		float32 anistropyStrength;
		float32 resolution;
	};

	// window settings
	struct WindowConfig {
		std::string title;
		std::string iconPath;

		uint32 width; // renderer width and height
		uint32 height;
		uint32 wWidth; // window width and height
		uint32 wHeight;
		
		bool resizable;
		bool maximized;
		bool borderless;
		bool fullscreen;
		bool alwaysOnTop;
		bool vSync;
	};

	// memory alloc config
	struct MemConfig {
		constexpr static size_t maxDescriptorSetLayouts = 4;
		constexpr static size_t maxDescriptorTypePerPool = 8;

		uint32 maxComponentCount;
		uint32 maxEntityCount;
	};

	struct GUIConfig {

	};

	AppConfig application;
	DebugConfig debug;
	RenderConfig rendering;
	WindowConfig window;
	MemConfig memory;
	GUIConfig gui;

private:
	Settings();

	friend const Settings& settings();
};

const Settings& settings();

} // namespace lyra
