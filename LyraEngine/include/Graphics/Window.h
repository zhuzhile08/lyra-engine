/*************************
 * @file Window.h
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

#include <SDL3/SDL.h>
#include <glm/glm.hpp>

namespace lyra {

namespace input {

void update();

} // namespace input

class Window {
public:
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
		
		alwaysOnTop = 0x00008000,
		skipTaskbar = 0x00010000,

		utility = 0x00020000,
		tooltip = 0x00040000,
		popupMenu = 0x00080000,

		allowHighDpi = 0x00002000,

		vulkan = 0x10000000,

		none = 0x00000000
	}; // refer to the docs of the current version of SDL for the documentation of these enums

	Window();
	Window(lsd::StringView title, Flags flags, const glm::ivec2& size);

	sdl::Window window;

	bool fullscreen = false;
	bool changed = false;

	friend void input::update();
};

void initWindow();
void initWindow(lsd::StringView title, Window::Flags flags, const glm::ivec2& size);

namespace renderer {

glm::uvec2 windowSize();
Window::Flags windowFlags();

} // namespace renderer

} // namespace lyra
