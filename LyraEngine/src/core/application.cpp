#include <core/application.h>

#include <core/rendering/window.h>
#include <core/rendering/render_system.h>

#include <core/rendering/vulkan/devices.h>

#include <init/init_SDL.h>
#include <core/logger.h>

namespace lyra {

Application::Application() {
	Logger::init();
	init_SDL();
	init();
}

Application::~Application() {
	quit_SDL();
	Logger::quit();
}

void Application::draw() {
	while (_window.running()) {
		_lastTime = std::move(_currentTime);

		_window.events();
		_renderSystem.update();
		_renderSystem.draw();

		_currentTime = SDL_GetTicks64();

		// framerate calculation
		_deltaTime = (_currentTime - _lastTime) * 0.001f;
		_fps = 1.0f / (float)_deltaTime;
	}

	_renderSystem.device()->wait();
}

Window Application::_window;
RenderSystem Application::_renderSystem(&_window);

} // namespace lyra