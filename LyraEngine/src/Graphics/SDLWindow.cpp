#include <Graphics/SDLWindow.h>

#include <SDL_error.h>
#include <backends/imgui_impl_sdl2.h>

#include <Common/Logger.h>
// #include <Common/Config.h>

namespace lyra {

Window::Window() noexcept {
	if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_EVENTS) != 0) {
		log::exception("SDL init error: {}!", SDL_GetError());
	}

	uint32 flags = SDL_WINDOW_VULKAN;

	/*
	if (config::fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
	if (config::maximized) flags |= SDL_WINDOW_MAXIMIZED;
	if (config::resizable) flags |= SDL_WINDOW_RESIZABLE;
	if (config::alwaysOnTop) flags |= SDL_WINDOW_ALWAYS_ON_TOP;
	if (config::borderless) flags |= SDL_WINDOW_BORDERLESS;
	*/

	// m_window = sdl::Window(config::title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, config::windowWidth, config::windowHeight, flags);

	m_window = sdl::Window("LyraEngine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 100, 100, flags);

	ASSERT(m_window, "Failed to create SDL window with error: {}!", SDL_GetError());
}

Window::~Window() noexcept {
	m_running = false;

	SDL_Quit();
}

}