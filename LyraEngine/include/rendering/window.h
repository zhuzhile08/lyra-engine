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

#include <functional>

#include <SDL.h>

#include <core/decl.h>
#include <core/queue.h>

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
	void destroy() noexcept {
		this->~Window();
	}

	Window(const Window&) noexcept = delete;
	Window operator=(const Window&) const noexcept = delete;

	/**
	 * @brief get the events
	 *
	 * @param eventType type of event occuring
	 * @param event that is occuring
	 */
	void events() noexcept;
	/**
	 * @brief add a function to the event queue
	 *
	 * @param function the function
	*/
	void check_events(std::function<void()>&& function);
	/**
	 * @brief wait until an event was detected
	 */
	void wait_events() { SDL_WaitEvent(&m_event); }

	/**
	 * @brief get the event object
	 * 
	 * @return const SDL_Event&
	 */
	NODISCARD const SDL_Event& event() const noexcept { return m_event; }
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
	NODISCARD const bool running() const noexcept { return m_running; }
	/**
	* @brief get if window was changed and set it back to false
	* 
	* @return const bool
	**/
	NODISCARD const bool changed() noexcept { return m_changed; }

private:
	SDL_Window* m_window;
	SDL_Event m_event;

	CallQueue m_eventQueue;

	bool m_fullscreen = false;
	bool m_running = true;
	bool m_changed = false;
};

} // namespace lyra
