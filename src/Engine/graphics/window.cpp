#include <graphics/window.h>

namespace lyra {

Window::Window() {
	LOG_INFO("Creating SDL window...");

	uint32 flags = SDL_WINDOW_VULKAN;

	if (fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
	if (resizable) flags |= SDL_WINDOW_RESIZABLE;

	_window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, flags);

	if (!_window) {
        LOG_EXEPTION("Failed to create SDL window with error: ", SDL_GetError());
	}

	LOG_INFO("Successfully created window at: ", GET_ADDRESS(this), "!", END_L);
}

void Window::destroy() noexcept {
	SDL_DestroyWindow(_window);

	LOG_INFO("Successfully destroyed SDL window!", TAB);
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

SDL_Window* Window::get() const noexcept {
	return _window;
}

}