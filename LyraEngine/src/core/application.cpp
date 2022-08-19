#include <core/application.h>

#include <core/rendering/window.h>
#include <core/rendering/render_system.h>

#include <core/rendering/vulkan/devices.h>

#include <init/init_SDL.h>
#include <core/logger.h>

#ifdef _WIN32
#include <windows.h>
#endif

namespace lyra {

Application::Application() {
	init();
}

Application::~Application() {
	quit_SDL();
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

void init(void) {
	// initialize SDL
	init_SDL();

#ifndef STDIO_SYNC
	std::ios::sync_with_stdio(true);
#endif

#ifdef _WIN32
	DWORD outMode = 0;
	HANDLE stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	GetConsoleMode(stdoutHandle, &outMode);
	outMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

	SetConsoleMode(stdoutHandle, outMode);
#endif
}

Window Application::_window;
RenderSystem Application::_renderSystem(&_window);

} // namespace lyra