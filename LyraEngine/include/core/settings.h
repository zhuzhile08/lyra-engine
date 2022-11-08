#pragma once

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

	// initialize the variables
	NODISCARD static std::string init_json();

	// json containing the variables
	static const nlohmann::json json;

	// generell application settings
	struct Application {
		static const char* description;
		static const int fps;

	private: 
		Application() noexcept = delete;
	};

	// debug settings
	struct Debug {
		static const DebugMode debug;
		static const DisableLog disableLog;
		static const bool printFPS;
		static const bool stdioSync;

		static const std::vector <const char*> requestedDeviceExtensions;
		static const std::vector <const char*> requestedValidationLayers;

	private:
		Debug() noexcept = delete;
	};

	// rendering settings
	struct Rendering {
		static const uint8 maxFramesInFlight;
		static const float fov;
		
		static const PolygonFrontFace polygonFrontFace;

		static const bool anistropy;
		static const float anistropyStrength;
		static const float resolution;

	private:
		Rendering() noexcept = delete;
	};

	// window settings
	struct Window {
		static const std::string title;
		static const std::string iconPath;

		static const uint32 width; // renderer width and height
		static const uint32 height;
		static const uint32 wWidth; // window width and height
		static const uint32 wHeight;
		
		static const bool resizable;
		static const bool maximized;
		static const bool borderless;
		static const bool fullscreen;
		static const bool alwaysOnTop;
		static const bool vSync;

	private:
		Window() noexcept = delete;
	};

	struct Memory {
		static const uint32 maxComponentCount;
		static const uint32 maxEntityCount;

		static const uint32 maxCommandBuffers; // caution: these only define the maximum amount of command buffers per pool
	};

	struct Gui {

	};

private:
	Settings() noexcept = delete;
};

} // namespace lyra
