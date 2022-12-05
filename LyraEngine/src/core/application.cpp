#include <core/application.h>

#include <input/input.h>

#include <rendering/window.h>
#include <rendering/render_system.h>

#include <rendering/vulkan/devices.h>

#include <init/init_SDL.h>

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
	while (window.running()) {
		m_lastTime = std::move(m_currentTime);
		
		Input::update();
		renderSystem.update();
		renderSystem.draw();

		m_currentTime = SDL_GetTicks64();

		// framerate calculation
		m_deltaTime = (m_currentTime - m_lastTime) * 0.001f;
		m_fps = 1.0f / (float)m_deltaTime;
	}

	renderSystem.device.wait();
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

Window Application::window;
RenderSystem Application::renderSystem(&window);

} // namespace lyra