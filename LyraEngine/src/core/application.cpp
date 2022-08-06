#include <core/application.h>

#include <core/rendering/window.h>
#include <core/rendering/render_system.h>

#include <core/rendering/vulkan/devices.h>

#include <init/init_SDL.h>
#include <core/logger.h>

namespace lyra {

Application::Application() {
	Logger::init();
	Settings::init();
	Context::init();
	init_SDL();
	init();
}

Application::~Application() {
	quit_SDL();
	Logger::quit();
}

void Application::draw() {
	while (Context::get()->window()->running()) {
		_lastTime = std::move(_currentTime);

		Context::get()->window()->events();
		Context::get()->renderSystem()->update();
		Context::get()->renderSystem()->draw();

		_currentTime = SDL_GetTicks64();

		// framerate calculation
		_deltaTime = (_currentTime - _lastTime) * 0.001f;
		_fps = 1.0f / (float)_deltaTime;
	}

	Context::get()->renderSystem()->device()->wait();
}

} // namespace lyra