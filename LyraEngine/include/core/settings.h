/*************************
 * @file settings.h
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

#include <lyra.h>

#include <vector>
#include <string>
#include <nlohmann/json.hpp>

namespace lyra {

struct Settings {
	// debug mode enum
	enum class DebugMode : int {
		MODE_DEBUG = 1,
		MODE_RELEASE = 2,
	};

	// disable logging enum
	enum class DisableLog {
		DISABLE_NONE = 0x00000001,
		DISABLE_DEBUG = 0x00000002,
		DISABLE_INFO = 0x00000004,
		DISABLE_WARN = 0x00000008,
		DISABLE_ERROR = 0x00000010,
		DISABLE_EXCEPTION = 0x00000020,
		DISABLE_ALL = 0x00000040
	};

	enum class PolygonFrontFace : int {
		FRONT_FACE_COUNTER_CLOCKWISE = 0,
		FRONT_FACE_CLOCKWISE = 1
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
		const uint8 maxFramesInFlight;
		const float fov;
		
		const PolygonFrontFace polygonFrontFace;

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
		const uint32 maxComponentCount;
		const uint32 maxEntityCount;

		const uint32 maxCommandBuffers; // caution: these only define the maximum amount of command buffers per pool
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
	Settings() noexcept :
		m_json(nlohmann::json::parse(init_json().c_str())),

		application{
			(char*)&m_json["application"]["description"],
			(int)m_json["application"]["fps"]
		},

		debug{
			static_cast<DebugMode>((int)m_json["debug"]["debug"]),
			static_cast<DisableLog>((int)m_json["debug"]["disableLog"]),
			(bool)m_json["debug"]["printFPS"],
			(bool)m_json["debug"]["stdioSync"],
			{ "VK_KHR_swapchain", "VK_KHR_portability_subset" },
			{ "VK_LAYER_KHRONOS_validation" }
		},

		rendering{
			(uint8)m_json["rendering"]["maxFramesInFlight"],
			(float)m_json["rendering"]["fov"],
			static_cast<PolygonFrontFace>((int)m_json["rendering"]["polygonFrontFace"]),
			(bool)m_json["rendering"]["anistropy"],
			(float)m_json["rendering"]["anistropyStrength"],
			(float)m_json["rendering"]["resolution"]
		},

		window{
			(std::string)m_json["window"]["title"],
			(std::string)m_json["window"]["iconPath"],
			(uint32)m_json["window"]["width"],
			(uint32)m_json["window"]["height"],
			(uint32)m_json["window"]["wWidth"],
			(uint32)m_json["window"]["wHeight"],
			(bool)m_json["window"]["resizable"],
			(bool)m_json["window"]["maximized"],
			(bool)m_json["window"]["borderless"],
			(bool)m_json["window"]["fullscreen"],
			(bool)m_json["window"]["alwaysOnTop"],
			(bool)m_json["window"]["vSync"]
		},

		memory{
			(uint32)m_json["memory"]["maxComponentCount"],
			(uint32)m_json["memory"]["maxEntityCount"],
			(uint32)m_json["memory"]["maxCommandBuffers"]
		}
	{ };

	friend const Settings& settings();
};

/**
 * @brief get a static settings object
 * 
 * @return const Settings& 
 */
const Settings& settings();

} // namespace lyra
