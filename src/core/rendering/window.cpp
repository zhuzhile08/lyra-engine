#include <core/rendering/window.h>

namespace lyra {

Window::Window() noexcept { }

Window::~Window() noexcept {
	SDL_DestroyWindow(_window);

	LOG_INFO("Successfully destroyed SDL window!", TAB);
}

void Window::destroy() noexcept {
	this->~Window();
}

void Window::create(uint32 width, uint32 height, bool resizable, bool fullscreen, const char* title) {
	LOG_INFO("Creating SDL window...");

	_width = width;
	_height = height;
	_resizable = resizable;
	_fullscreen = fullscreen;
	_title = title;

	uint32 flags = SDL_WINDOW_VULKAN;

	if (_fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
	if (_resizable) flags |= SDL_WINDOW_RESIZABLE;

	_window = SDL_CreateWindow(_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _width, _height, flags);

	if (!_window) {
		LOG_EXEPTION("Failed to create SDL window with error: ", SDL_GetError());
	}

	LOG_INFO("Successfully created window at: ", get_address(this), "!", END_L);
}

void Window::events() noexcept {
	while (SDL_PollEvent(&_event)) {
		/// @todo
		
	}
}

bool Window::get_events(const uint32 eventType, const uint32 event) noexcept {
	/// @todo
	return false;
}

}