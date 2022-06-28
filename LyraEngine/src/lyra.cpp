#include <lyra.h>

namespace lyra {

void Application::init() noexcept {
	init_SDL();

	_window.create(Settings::Window::width, Settings::Window::height, Settings::Window::resizable, Settings::Window::fullscreen, Settings::Window::title);
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