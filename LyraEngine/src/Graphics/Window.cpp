#include <Graphics/Window.h>

#include <SDL3/SDL_error.h>
#include <backends/imgui_impl_sdl3.h>

#include <Common/Logger.h>
#include <Common/Config.h>

namespace lyra {

namespace renderer {

Window* globalWindow;

}

Window::Window() {
	uint32 flags = SDL_WINDOW_VULKAN;

	if (config::fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
	if (config::maximized) flags |= SDL_WINDOW_MAXIMIZED;
	if (config::resizable) flags |= SDL_WINDOW_RESIZABLE;
	if (config::alwaysOnTop) flags |= SDL_WINDOW_ALWAYS_ON_TOP;
	if (config::borderless) flags |= SDL_WINDOW_BORDERLESS;

	window = sdl::Window(config::title, config::windowWidth, config::windowHeight, flags);

	ASSERT(window, "Failed to create SDL window with error: {}!", SDL_GetError());

	SDL_SetWindowRelativeMouseMode(window, SDL_TRUE);
}

Window::Window(lsd::StringView title, Flags flags, const glm::ivec2& size) {
	window = sdl::Window(title, size.x, size.y, static_cast<uint32>(flags));

	ASSERT(window, "Failed to create SDL window with error: {}!", SDL_GetError());
}

void initWindow() {
	if (renderer::globalWindow)
		log::error("lyra::initWindow(): The window is already initialized!");
	else
		renderer::globalWindow = new Window();
}
void initWindow(lsd::StringView title, Window::Flags flags, const glm::ivec2& size) {
	if (renderer::globalWindow)
		log::error("lyra::initWindow(): The window is already initialized!");
	else
		renderer::globalWindow = new Window(title, flags, size);
}

namespace renderer {

glm::uvec2 windowSize() {
	glm::ivec2 r;
	SDL_GetWindowSizeInPixels(renderer::globalWindow->window, &r.x, &r.y);

	return r;
}

Window::Flags windowFlags() {
	return static_cast<Window::Flags>(SDL_GetWindowFlags(renderer::globalWindow->window));
}

} // 

} // namespace lyra
