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
		_window.events();
		_context.update();
		_context.draw();
	}

	_context.device()->wait();
}

Window Application::_window;
Context Application::_context;

} // namespace lyra