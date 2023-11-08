#include <Graphics/SDLWindow.h>

#include <SDL_error.h>
#include <backends/imgui_impl_sdl3.h>

#include <Common/Logger.h>
// #include <Common/Config.h>

namespace lyra {

Window::Window() {
	uint32 flags = SDL_WINDOW_VULKAN;

	if (config::fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
	if (config::maximized) flags |= SDL_WINDOW_MAXIMIZED;
	if (config::resizable) flags |= SDL_WINDOW_RESIZABLE;
	if (config::alwaysOnTop) flags |= SDL_WINDOW_ALWAYS_ON_TOP;
	if (config::borderless) flags |= SDL_WINDOW_BORDERLESS;

	m_window = sdl::Window(config::title, config::windowWidth, config::windowHeight, flags);

	ASSERT(m_window, "Failed to create SDL window with error: {}!", SDL_GetError());
}

Window::Window(std::string_view title, Flags flags, const glm::ivec2& size) {
	m_window = sdl::Window(title, size.x, size.y, static_cast<uint32>(flags));

	ASSERT(m_window, "Failed to create SDL window with error: {}!", SDL_GetError());
}

std::vector<const char*> Window::instanceExtensions() const {
	uint32 instanceExtensionCount;
	auto extensions = SDL_Vulkan_GetInstanceExtensions(&instanceExtensionCount);
	
	std::vector<const char*> instanceExtensions(extensions, extensions + instanceExtensionCount);

	return instanceExtensions;
}

glm::uvec2 Window::getDrawableSize() const {
	glm::ivec2 r;
	SDL_GetWindowSizeInPixels(m_window, &r.x, &r.y);

	return r;
}

uint32 Window::getWindowFlags() const {
	return SDL_GetWindowFlags(m_window);
}

} // namespace lyra
