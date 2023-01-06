#include <rendering/window.h>

#include <SDL_error.h>
#include <backends/imgui_impl_sdl.h>

#include <core/settings.h>


namespace lyra {

Window::Window() noexcept {
	uint32 flags = SDL_WINDOW_VULKAN;

	if (settings().window.fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
	if (settings().window.maximized) flags |= SDL_WINDOW_MAXIMIZED;
	if (settings().window.resizable) flags |= SDL_WINDOW_RESIZABLE;
	if (settings().window.alwaysOnTop) flags |= SDL_WINDOW_ALWAYS_ON_TOP;
	if (settings().window.borderless) flags |= SDL_WINDOW_BORDERLESS;

	m_window = SDL_CreateWindow(settings().window.title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, settings().window.wWidth, settings().window.wHeight, flags);

	lassert(m_window, "Failed to create SDL window with error: ", SDL_GetError());
}

Window::~Window() noexcept {
	m_running = false;

	SDL_DestroyWindow(m_window);
}

}