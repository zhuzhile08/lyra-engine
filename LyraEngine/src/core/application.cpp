#include <core/application.h>

#include <input/input.h>

#include <rendering/window.h>
#include <rendering/render_system.h>

#include <rendering/vulkan/devices.h>

#include <init/init_SDL.h>

#include <utility>

namespace lyra {

Application::Application() {
	this->init();
}

Application::~Application() {
	quit_SDL();
}

void Application::draw() {
	while (window.running()) {
		m_lastTime = std::move(m_currentTime);
		
		input::InputManager::update();

		renderSystem.draw();

		m_currentTime = SDL_GetTicks64();
	}

	// wait for everything to finish before destruction
	renderSystem.device.wait();
}

Window Application::window;
RenderSystem Application::renderSystem(&window);

} // namespace lyra