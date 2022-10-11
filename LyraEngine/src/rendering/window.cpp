#include <rendering/window.h>

#include <SDL_error.h>
#include <backends/imgui_impl_sdl.h>

#include <core/settings.h>
#include <core/logger.h>
#include <core/util.h>

namespace lyra {

Window::Window() noexcept {
	Logger::log_info("Creating SDL window...");

	uint32 flags = SDL_WINDOW_VULKAN;

	if (Settings::Window::fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
	if (Settings::Window::maximized) flags |= SDL_WINDOW_MAXIMIZED;
	if (Settings::Window::resizable) flags |= SDL_WINDOW_RESIZABLE;
	if (Settings::Window::alwaysOnTop) flags |= SDL_WINDOW_ALWAYS_ON_TOP;
	if (Settings::Window::borderless) flags |= SDL_WINDOW_BORDERLESS;

	m_window = SDL_CreateWindow(Settings::Window::title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Settings::Window::wWidth, Settings::Window::wHeight, flags);

	lassert(m_window, "Failed to create SDL window with error: ", SDL_GetError());

	Logger::log_info("Successfully created window at: ", get_address(this), "!", Logger::end_l());
}

Window::~Window() noexcept {
	m_running = false;

	SDL_DestroyWindow(m_window);

	Logger::log_info("Successfully destroyed SDL window!", Logger::tab());
}

}