/*************************
 * @file SDLWindow.h
 *
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief wrapper around the Window
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
#include <glm/glm.hpp>

namespace lyra {

namespace input {

class Input;

} // namespace input

class Window {
public:
	static constexpr int centered = SDL_WINDOWPOS_CENTERED;

	enum class Flags {
		fullscreen = 0x00000001,
		fullscreenDesktop = ( fullscreen | 0x00001000 ),
		hidden = 0x00000008,

		borderless = 0x00000010,
		resizable = 0x00000020,
		minimized = 0x00000040,
		maximized = 0x00000080,

		mouseGrabbed = 0x00000100,
		inputFocus = 0x00000200,
		mouseFocus = 0x00000400,
		mouseCapture = 0x00004000,
		keyboardGrabbed = 0x00100000,

		inputGrabbed = mouseGrabbed,
		
		alwaysOnTop = 0x00008000,
		skipTaskbar = 0x00010000,

		utility = 0x00020000,
		tooltip = 0x00040000,
		popupMenu = 0x00080000,

		allowHighDpi = 0x00002000,

		vulkan = 0x10000000
	}; // refer to the docs of the current version of SDL for the documentation of these enums

	// construct a window in default engine configuration
	Window();
	// construct a window with custom settings
	Window(std::string_view title, Flags flags, const glm::ivec2& size, const glm::ivec2& position = {centered, centered});

	virtual ~Window() noexcept {
		m_running = false;
	}

	std::vector<const char*> instanceExtensions() const;
	glm::uvec2 getDrawableSize() const;
	uint32 getWindowFlags() const;

	NODISCARD constexpr SDL_Window* get() const noexcept { return m_window; }
	NODISCARD constexpr bool running() const noexcept { return m_running; }
	NODISCARD constexpr bool& running() noexcept { return m_running; }
	NODISCARD constexpr bool changed() const noexcept { return m_changed; }

private:
	sdl::Window m_window;

	bool m_fullscreen = false;
	bool m_running = true;
	bool m_changed = false;

	friend class input::Input;
};

} // namespace lyra
