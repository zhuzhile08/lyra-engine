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

#include <vector>
#include <string>
#include <nlohmann/json.hpp>

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
		const char* description;
		const int fps;
	};

	// debug settings
	struct DebugConfig {
		const DebugMode debug;
		const DisableLog disableLog;
		const bool printFPS;
		const bool stdioSync;

		const std::vector <const char*> requestedDeviceExtensions;
		const std::vector <const char*> requestedValidationLayers;
	};

	// rendering settings
	struct RenderConfig {
		constexpr static size_t maxFramesInFlight = 2;
		constexpr static size_t maxSwapchainImages = 8;
		constexpr static size_t maxConcurrentRenderers = 16;

		const float fov;

		const bool anistropy;
		const float anistropyStrength;
		const float resolution;
	};

	// window settings
	struct WindowConfig {
		const std::string title;
		const std::string iconPath;

		const uint32 width; // renderer width and height
		const uint32 height;
		const uint32 wWidth; // window width and height
		const uint32 wHeight;
		
		const bool resizable;
		const bool maximized;
		const bool borderless;
		const bool fullscreen;
		const bool alwaysOnTop;
		const bool vSync;
	};

	// memory alloc config
	struct MemConfig {
		constexpr static size_t maxDescriptorSetLayouts = 4;
		constexpr static size_t maxDescriptorTypePerPool = 8;

		const uint32 maxComponentCount;
		const uint32 maxEntityCount;
	};

	struct GUIConfig {

	};

private:
	/**
	 * @brief load the m_json
	 * 
	 * @return std::string
	 */
	NODISCARD std::string init_json() const;

	nlohmann::json m_json;

public:
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

/**
 * @brief get a static settings object
 * 
 * @return const Settings& 
 */
const Settings& settings();

} // namespace lyra
