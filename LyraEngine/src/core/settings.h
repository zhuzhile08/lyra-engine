#pragma once

#include <core/defines.h>

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
	struct Application {
		static constexpr const char* description = "";
		static constexpr int fps = 60;

	private: 
		Application() noexcept = delete;
	};

	// debug settings
	struct Debug {
		static constexpr DebugMode debug = DebugMode::MODE_DEBUG;
		static constexpr DisableLog disableLog = DisableLog::DISABLE_NONE;
		static constexpr bool printFPS = false;
		static constexpr bool stdio_sync = false;

		static const std::vector <const char*> requestedDeviceExtensions;
		static const std::vector <const char*> requestedValidationLayers;

	private:
		Debug() noexcept = delete;
	};

	// rendering settings
	struct Rendering {
		static constexpr uint8 maxFramesInFlight = 2;
		static constexpr float fov = 70.f;
		static constexpr PolygonFrontFace polygonFrontFace = PolygonFrontFace::FRONT_FACE_COUNTER_CLOCKWISE;

	private:
		Rendering() noexcept = delete;
	};

	// window settings
	struct Window {
		static constexpr const char* title = "Game";
		static constexpr const char* icon = "data/img/icon.png";

		static constexpr uint32 width = 1024;
		static constexpr uint32 height = 600;
		static constexpr uint32 resolution = 1;

		static constexpr bool resizable = true;
		static constexpr bool borderless = false;
		static constexpr bool fullscreen = false;
		static constexpr bool alwaysOnTop = false;
		static constexpr bool vSync = true;

	private:
		Window() noexcept = delete;
	};

	struct Gui {

	};

	Settings() noexcept = delete;
};

#define LOG_FILE

} // namespace lyra
