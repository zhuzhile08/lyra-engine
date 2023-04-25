#include <Input/Input.h>

#include <backends/imgui_impl_sdl2.h>

#include <Application/Application.h>

namespace lyra {

namespace input {

void InputManager::update() {
	Application::window.m_changed = false;

	// get new events and set the passed events
	SDL_PumpEvents();
	m_mouseState = SDL_GetMouseState(&m_mousePos.x, &m_mousePos.y);

	// poll fixed events
	while (SDL_PollEvent(&m_events)) {

		// ImGui_ImplSDL2_ProcessEvent(&m_events);

		// default events that are always checked
		switch (m_events.type) {
			case SDL_QUIT: // quitting	
				Application::window.m_running = false;
			case SDL_WINDOWEVENT: // window events like resizing
				if (m_events.window.event == SDL_WINDOWEVENT_SIZE_CHANGED || m_events.window.event == SDL_WINDOWEVENT_MINIMIZED || m_events.window.event == SDL_WINDOWEVENT_MAXIMIZED)
					Application::window.m_changed = true;
					/// @todo: window fullscreen not registering
		}
	}
}

int InputManager::m_keyCount;
const uint8* InputManager::m_keyboardState = SDL_GetKeyboardState(&InputManager::m_keyCount);

uint32 InputManager::m_mouseState = SDL_GetMouseState(&InputManager::m_mousePos.x, &InputManager::m_mousePos.y);
glm::ivec2 InputManager::m_mousePos;

SDL_Event InputManager::m_events;

} // namespace input

} // namespace lyra
