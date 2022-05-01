/*************************
 * @file lyra.h
 *
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief wrapper the basic application features
 *
 * @date 2022-04-30
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <optional>

#include <init/init_SDL.h>
#include <core/rendering/window.h>
#include <core/rendering/context.h>
#include <core/rendering/gui_context.h>
#include <core/logger.h>
#include <core/defines.h>

namespace lyra {

struct InitInfo {
	const uint32 width; // width of the window
	const uint32 height; // height of the window
	const char* title = "Game"; // title of the window
	const bool gui = true; // window gui
	const bool resizable = false; // window resizable
	const bool fullscreen = false; // window fullscreen
	const std::vector<string> reqValidLayers{ "VK_LAYER_KHRONOS_validation" }; // requested Vulkan validation layers
	const std::vector<string> reqDevExt{ VK_KHR_SWAPCHAIN_EXTENSION_NAME }; // requested Vulkan device extensions
	const uint8 framesInFlight = 2; // number of frames(images)
};

// wrapper around a every basic component of an application
class Application {
public: // behold, peasant, my superior "singleton" architecture
	/**
	 * @brief destructor of the window
	**/
	virtual ~Application() noexcept;

	Application(const Application&) noexcept = delete;
	Application operator=(const Application&) const noexcept = delete;

	/**
	 * @brief initialize a new application
	 *
	 * @param InitInfo initalization information
	*/
	static void init(InitInfo info) noexcept;

	/**
	 * @brief get the window
	 *
	 * @return static const lyra::Window* const
	 */
	[[nodiscard]] static const Window* const window() noexcept { return &_window; };
	/**
	 * @brief get the application context
	 *
	 * @return static lyra::Context* const
	 */
	[[nodiscard]] static Context* const context() noexcept { return &_context; };
	/**
	 * @brief get the GUI context
	 *
	 * @return static const lyra::gui::GUIContext* const
	 */
	[[nodiscard]] static const gui::GUIContext* const guiContext() noexcept { 
		if (_guiContext.has_value() == true) return &_guiContext.value(); // check if the context has a value or not
		else {
			LOG_EXEPTION("Current application does not have a context for a ImGui based GUI");
			return nullptr;
		}
	};

private:
	Application() noexcept;

	static Window _window;
	static Context _context;
	static std::optional<gui::GUIContext> _guiContext;
};

} // namespace lyra