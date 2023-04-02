/*************************
 * @file SDLWindow.h
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

#include <Lyra/Lyra.h>

#include <functional>

#include <SDL.h>

#include <Common/RAIIContainers.h>

namespace lyra {

struct WindowEvents {
};


namespace input {
class InputManager;
} // namespace input


/**
 * @brief wrapper around a SDL window with some quality of life improvements
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

	Window(const Window&) noexcept = delete;
	Window operator=(const Window&) const noexcept = delete;

	/**
	 * @brief get the window object
	 *
	 * @return SDL_Window*
	 */
	NODISCARD SDL_Window* get() const noexcept { return m_window; };
	/**
	* @brief get if window is still running
	* 
	* @return const bool
	**/
	NODISCARD constexpr bool running() const noexcept { return m_running; }
	/**
	* @brief get if window was changed and set it back to false
	* 
	* @return const bool
	**/
	NODISCARD constexpr bool changed() noexcept { return m_changed; }

private:
	sdl::Window m_window;

	bool m_fullscreen = false;
	bool m_running = true;
	bool m_changed = false;

	friend class input::InputManager;
};

} // namespace lyra
