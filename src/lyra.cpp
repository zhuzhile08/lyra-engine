#include <lyra.h>

namespace lyra {

Application::Application() noexcept { }

Application::~Application() {
	_guiContext.reset();
	quit_SDL();
}

void Application::init(InitInfo info) noexcept {
	init_SDL();

	_window.create(info.width, info.height, info.resizable, info.fullscreen, info.title);
	_context.create(&_window);
	if (info.gui == true) _guiContext->create(&_context, &_window);
}

Window Application::_window;
Context Application::_context;
std::optional<gui::GUIContext> Application::_guiContext;

} // namespace lyra