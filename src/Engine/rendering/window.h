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

/**
 * @brief wrapper around a SDL_Window with some quality of life improvements
 * 
 * @todo get more events and window settings. The current window is VERY barebones and only the bone of the entire structure is implemented
 */
class Window {
private:
	SDL_Window* window;

	int         width       = WIDTH;
	int         height      = HEIGHT;
	bool        resizable   = RESIZEABLE;
	bool        fullscreen  = FULLSCREEN;
	const char* title       = TITLE;

	bool 		resized 	= false;

	/**
	 * @brief destroy the window
	 */
	void        destroy();
	/**
	 * @brief get window events like resizing
	 */
	void 		events(SDL_Event event);

public:
	/**
	 * @brief construct a new window
	 */
	Window();

	/**
	 * @brief get the window object
	 * 
	 * @return SDL_Window* 
	 */
	SDL_Window* get_window() 	const;
	/**
	 * @brief get if window was resized
	 * 
	 * @return bool 
	 */
	bool		get_resized()	const;
};

} // namespace lyra
