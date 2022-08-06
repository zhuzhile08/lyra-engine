#pragma once

#include <core/util.h>
#include <res/loaders/load_file.h>

#include <json.hpp>
#include <vector>

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
	static void init();

	// generell application settings
	struct Application {
		static char* description;
		static int fps;

	private: 
		Application() noexcept = delete;
	};

	// debug settings
	struct Debug {
		static DebugMode debug;
		static DisableLog disableLog;
		static bool printFPS;
		static bool stdioSync;

		static std::vector <const char*> requestedDeviceExtensions;
		static std::vector <const char*> requestedValidationLayers;

	private:
		Debug() noexcept = delete;
	};

	// rendering settings
	struct Rendering {
		static uint8 maxFramesInFlight;
		static float fov;
		
		static PolygonFrontFace polygonFrontFace;

		static bool anistropy;
		static float anistropyStrength;
		static float resolution;

	private:
		Rendering() noexcept = delete;
	};

	// window settings
	struct Window {
		static std::string title;
		static std::string iconPath;

		static uint32 width; // renderer width and height
		static uint32 height;
		static uint32 wWidth; // window width and height
		static uint32 wHeight;
		
		static bool resizable;
		static bool maximized;
		static bool borderless;
		static bool fullscreen;
		static bool alwaysOnTop;
		static bool vSync;

	private:
		Window() noexcept = delete;
	};

	struct Memory {
		static uint32 maxComponentCount;
		static uint32 maxEntityCount;

		static uint32 maxCommandBuffers; // caution: these only define the maximum amount of command buffers per pool
	};

	struct Gui {

	};

private:
	Settings() noexcept = delete;
};

} // namespace lyra
