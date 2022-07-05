#include <lyra.h>

namespace lyra {

void Application::init() noexcept {
	Logger::init();
	Settings::init();
	init_SDL();

	_window.create();
	_context.create(&_window);
}

void Application::draw() {
	while (_window.running()) {
		_lastTime = _currentTime;

		_window.events();
		_context.update();
		_context.draw();

		_currentTime = SDL_GetTicks64();

		// framerate calculation
		_deltaTime = (_currentTime - _lastTime) * 0.001f;
		_fps = 1.0f / (float)_deltaTime;

		Logger::log_debug("FPS: ", _fps);
	}

	_context.device()->wait();
}

float Application::_fps;
float Application::_deltaTime;
Window Application::_window;
Context Application::_context;
uint64 Application::_lastTime = SDL_GetTicks64();
uint64 Application::_currentTime;

} // namespace lyra