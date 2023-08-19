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

#include <Common/Common.h>
#include <Common/RAIIContainers.h>

#include <SDL.h>

namespace lyra {

namespace input {

class Input;

} // namespace input

class Window {
public:
	Window() noexcept;
	virtual ~Window() noexcept;

	Window(const Window&) noexcept = delete;
	Window operator=(const Window&) const noexcept = delete;

	NODISCARD SDL_Window* get() const noexcept { return m_window; };
	NODISCARD constexpr bool running() const noexcept { return m_running; }
	NODISCARD constexpr bool changed() const noexcept { return m_changed; }

private:
	sdl::Window m_window;

	bool m_fullscreen = false;
	bool m_running = true;
	bool m_changed = false;

	friend class input::Input;
};

} // namespace lyra
