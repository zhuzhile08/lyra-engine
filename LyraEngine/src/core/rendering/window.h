/*************************
 * @file window.h
 *
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief wrapper around the SDL_Window
 * @brief gets some window related events
 *
 * @date 2022-02-24
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <core/defines.h>
#include <math/math.h>
#include <core/logger.h>

#include <SDL.h>
#include <SDL_error.h>

namespace lyra {

struct WindowEvents {
};

/**
 * @brief wrapper around a SDL_Window with some quality of life improvements
 *
 * @todo get more events and window settings. The current window is VERY barebones and only the bone of the entire structure is implemented
 */
class Window {
public:
	/**
	 * @brief construct a new window
	 */
	Window() noexcept;

	/**
	* @brief destructor of the window
	**/
	virtual ~Window() noexcept;

	/**
	 * @brief destroy the window
	 */
	void destroy() noexcept;

	Window(const Window&) noexcept = delete;
	Window operator=(const Window&) const noexcept = delete;

	/**
	 * @brief construct a new window object
	 *
	 * @param width width of the window
	 * @param height height of the window
	 * @param resizable is window is resizeable
	 * @param fullscreen is window fullscreen
	 * @param title title of the window
	*/
	void create(uint32 width, uint32 height, bool resizable, bool fullscreen, const char* title);

	/**
	 * @brief get the events
	 *
	 * @param eventType type of event occuring
	 * @param event that is occuring
	 */
	void events() noexcept;

	/**
	 * @brief get the event object
	 * 
	 * @return const SDL_Event
	 */
	[[nodiscard]] const SDL_Event event() const noexcept { return _event; }
	/**
	 * @brief get the window object
	 *
	 * @return SDL_Window*
	 */
	[[nodiscard]] SDL_Window* get() const noexcept { return _window; };
	/**
	* @brief get if window is still running
	* 
	* @return const bool
	**/
	[[nodiscard]] const bool running() const noexcept { return _running; }

private:
	SDL_Window* _window;
	SDL_Event _event;

	uint32 _width = 0;
	uint32 _height = 0;
	bool _resizable = false;
	bool _fullscreen = false;
	bool _running = true;
	const char* _title = "Game";
};

} // namespace lyra
