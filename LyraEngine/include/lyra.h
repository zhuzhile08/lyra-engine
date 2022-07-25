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

#include <core/settings.h>

#include <core/rendering/window.h>
#include <core/rendering/context.h>

#include <core/rendering/vulkan/devices.h>

namespace lyra {

// @brief wrapper around a every basic component of an application
class Application {
public: // behold, peasant, my superior "singleton" architecture
	Application() noexcept = delete;
	Application(const Application&) noexcept = delete;
	Application operator=(const Application&) const noexcept = delete;

	/**
	 * @brief initialize a new application
	*/
	static void init() noexcept;

	/**
	 * @brief quit the application
	 */
	static void quit();

	/**
	 * @brief draw the current frame
	*/
	static void draw();

	/**
	 * @brief get the fps
	 *
	 * @return static const float
	 */
	[[nodiscard]] static const float fps() noexcept { return _fps; }
	/**
	 * @brief get the deltatime
	 *
	 * @return static const float
	 */
	[[nodiscard]] static const float deltaTime()  noexcept { return _deltaTime; }
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

private:
	static float _fps;
	static float _deltaTime;
	static Window _window;
	static Context _context;

	static uint64 _lastTime;
	static uint64 _currentTime;
};

} // namespace lyra