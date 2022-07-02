#include <core/rendering/window.h>

namespace lyra {

void Window::create() noexcept {
	Logger::log_info("Creating SDL window...");

	uint32 flags = SDL_WINDOW_VULKAN;

	if (Settings::Window::fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
	if (Settings::Window::resizable) flags |= SDL_WINDOW_RESIZABLE;
	if (Settings::Window::alwaysOnTop) flags |= SDL_WINDOW_ALWAYS_ON_TOP;
	if (Settings::Window::borderless) flags |= SDL_WINDOW_BORDERLESS;

	_window = SDL_CreateWindow(Settings::Window::title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Settings::Window::width, Settings::Window::height, flags);

	if (!_window) {
		Logger::log_exception("Failed to create SDL window with error: ", SDL_GetError());
	}

	Logger::log_info("Successfully created window at: ", get_address(this), "!", Logger::end_l());
}

void Window::events() noexcept {
	if (SDL_PollEvent(&_event)) {
		_changed = false;
		_eventQueue.flush();

		// check for quitting
		if (_event.type == SDL_QUIT)
		{
			quit();
		}

		if (_event.type == SDL_WINDOWEVENT) {
			if (_event.window.event == SDL_WINDOWEVENT_RESIZED) {
				_changed = true;
			}
		}
	}
}

}