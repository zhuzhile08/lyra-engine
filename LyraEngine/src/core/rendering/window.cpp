#include <core/rendering/window.h>

#include <core/settings.h>

#include <core/logger.h>
#include <backends/imgui_impl_sdl.h>

#include <SDL_error.h>

namespace lyra {

Window::Window() noexcept {
	Logger::log_info("Creating SDL window...");

	_eventQueue = new CallQueue;

	uint32 flags = SDL_WINDOW_VULKAN;

	if (Settings::Window::fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
	if (Settings::Window::maximized) flags |= SDL_WINDOW_MAXIMIZED;
	if (Settings::Window::resizable) flags |= SDL_WINDOW_RESIZABLE;
	if (Settings::Window::alwaysOnTop) flags |= SDL_WINDOW_ALWAYS_ON_TOP;
	if (Settings::Window::borderless) flags |= SDL_WINDOW_BORDERLESS;

	_window = SDL_CreateWindow(Settings::Window::title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Settings::Window::wWidth, Settings::Window::wHeight, flags);

	lassert(_window, "Failed to create SDL window with error: ", SDL_GetError());

	Logger::log_info("Successfully created window at: ", get_address(this), "!", Logger::end_l());
}

Window::~Window() noexcept {
	_running = false;

	delete _eventQueue;
	SDL_DestroyWindow(_window);

	Logger::log_info("Successfully destroyed SDL window!", Logger::tab());
}

void Window::check_events(std::function<void()>&& function) {
	_eventQueue->add(std::move(function));
}

void Window::events() noexcept {
	if (SDL_PollEvent(&_event)) {
		_changed = false;
		_eventQueue->flush();

		// check for quitting
		if (_event.type == SDL_QUIT)
		{
			_running = false;
		}

		if (_event.type == SDL_WINDOWEVENT) {
			if (_event.window.event == SDL_WINDOWEVENT_RESIZED) {
				_changed = true;
			}
		}
	}
}

}