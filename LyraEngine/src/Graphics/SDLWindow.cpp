#include <Graphics/SDLWindow.h>

#include <SDL_error.h>
#include <backends/imgui_impl_sdl2.h>

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

	m_window = sdl::Window(config::title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, config::windowWidth, config::windowHeight, flags);

	ASSERT(m_window, "Failed to create SDL window with error: {}!", SDL_GetError());
}

Window::Window(std::string_view title, Flags flags, const glm::ivec2& size, const glm::ivec2& position) {
	m_window = sdl::Window(title, position.x, position.y, size.x, size.y, static_cast<uint32>(flags));

	ASSERT(m_window, "Failed to create SDL window with error: {}!", SDL_GetError());
}

std::vector<const char*> Window::getInstanceExtensions() const {
	uint32 instanceExtensionCount = 0;
	ASSERT(SDL_Vulkan_GetInstanceExtensions(m_window, &instanceExtensionCount, nullptr) == SDL_TRUE, "Failed to get number of Vulkan instance extensions");
	std::vector<const char*> instanceExtensions(instanceExtensionCount);
	ASSERT(SDL_Vulkan_GetInstanceExtensions(m_window, &instanceExtensionCount, instanceExtensions.data()) == SDL_TRUE, "Failed to get Vulkan instance extensions");

	return instanceExtensions;
}

glm::uvec2 Window::getDrawableSize() const {
	glm::ivec2 r;
	SDL_Vulkan_GetDrawableSize(m_window, &r.x, &r.y);
	return r;
}

uint32 Window::getWindowFlags() const {
	return SDL_GetWindowFlags(m_window);
}

} // namespace lyra
