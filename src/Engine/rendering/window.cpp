#include <rendering/window.h>

namespace lyra {

Window::Window() {
	uint32 flags = SDL_WINDOW_VULKAN;

	if (fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
	if (resizable) flags |= SDL_WINDOW_RESIZABLE;

	window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, flags);

	if (!window) {
        LOG_EXEPTION("SDL create window", SDL_GetError());
	}
}

void Window::destroy() {
	SDL_DestroyWindow(window);
}

void Window::events(SDL_Event event) {
	if (event.type == SDL_WINDOWEVENT) {
		switch(event.window.event) {
			case SDL_WINDOWEVENT_RESIZED:
				LOG_INFO("SDL Window was resized");
				resized = true;
		}
	}
}

SDL_Window* Window::get_window() 		const {
	return window;
}

}