#include <Graphics/SDLWindow.h>

#include <SDL_error.h>
#include <backends/imgui_impl_sdl2.h>

#include <Common/Logger.h>
#include <Common/Settings.h>

namespace lyra {

Window::Window() noexcept {
	if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_EVENTS) != 0) {
		log().exception("SDL init error: ", SDL_GetError());
	}

	uint32 flags = SDL_WINDOW_VULKAN;

	if (settings().window.fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
	if (settings().window.maximized) flags |= SDL_WINDOW_MAXIMIZED;
	if (settings().window.resizable) flags |= SDL_WINDOW_RESIZABLE;
	if (settings().window.alwaysOnTop) flags |= SDL_WINDOW_ALWAYS_ON_TOP;
	if (settings().window.borderless) flags |= SDL_WINDOW_BORDERLESS;

	m_window = sdl::Window(settings().window.title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, settings().window.wWidth, settings().window.wHeight, flags);

	lassert(m_window, "Failed to create SDL window with error: ", SDL_GetError());
}

Window::~Window() noexcept {
	m_running = false;

	SDL_Quit();
}

}