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
 * 
 */

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
	Window();

	/**
	 * @brief get the events
	 *
	 * @param eventType type of event occuring
	 * @param event that is occuring
	 */
	void events() noexcept;

	/**
	 * @brief get the events
	 *
	 * @param eventType type of event occuring
	 * @param event that is occuring
	 */
	bool get_events(const uint32 eventType, const uint32 event) noexcept;

	/**
	 * @brief get the window object
	 *
	 * @return SDL_Window*
	 */
	SDL_Window* get() const noexcept;

private:
	SDL_Window* _window;
	SDL_Event _event;

	int width = WIDTH;
	int height = HEIGHT;
	bool resizable = RESIZEABLE;
	bool fullscreen = FULLSCREEN;
	const char* title = TITLE;

	/**
	 * @brief destroy the window
	 */
	void destroy() noexcept;
};

} // namespace lyra
