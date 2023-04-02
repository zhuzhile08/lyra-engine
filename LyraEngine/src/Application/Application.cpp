#include <Application/Application.h>

#include <Input/Input.h>

#include <Graphics/VulkanImpl/Device.h>

#include <utility>

namespace lyra {

Application::Application() {
	this->init();
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