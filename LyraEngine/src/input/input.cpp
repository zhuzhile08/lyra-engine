#include <input/input.h>

#include <backends/imgui_impl_sdl.h>

namespace lyra {

const bool Input::check_key(Keyboard event) {
	return m_keyboardState[static_cast<SDL_Scancode>(event)];
}

const bool Input::check_key_down(Keyboard event) {
	return m_keyboardState[static_cast<SDL_Scancode>(event)] && !m_prevKeyboardState[static_cast<SDL_Scancode>(event)];
}

const bool Input::check_key_up(Keyboard event) {
	return !m_keyboardState[static_cast<SDL_Scancode>(event)] && m_prevKeyboardState[static_cast<SDL_Scancode>(event)];
}

const bool Input::check_mouse_button(Mouse mouse) {
	return (m_mouseState & static_cast<uint8>(mouse)) != 0;
}

const bool Input::check_mouse_button_down(Mouse mouse) {
	return (m_mouseState & static_cast<uint8>(mouse)) && !(m_prevMouseState & static_cast<uint8>(mouse));
}

const bool Input::check_mouse_button_up(Mouse mouse) {
	return !(m_mouseState & static_cast<uint8>(mouse)) && (m_prevMouseState & static_cast<uint8>(mouse));
}

const glm::vec2 Input::check_mouse_position() {
	return m_mousePos;
}	

void Input::check_mouse_position(glm::vec2& mousePos) {
	mousePos = m_mousePos;
}

void Input::update() {
	// get new events and set the passed events
	memcpy(m_prevKeyboardState, m_keyboardState, m_keyCount);
	m_prevMouseState = m_mouseState;
	SDL_PumpEvents();
	m_mouseState = SDL_GetMouseState(&m_mousePos.x, &m_mousePos.y);

	// poll fixed events
	while (SDL_PollEvent(&m_events)) {
		Application::window()->m_changed = false;

		ImGui_ImplSDL2_ProcessEvent(&m_events);

		// default events that are always checked
		switch (m_events.type) {
			case SDL_QUIT: // quitting
				Application::window()->m_running = false;
			case SDL_WINDOWEVENT: // window events like resizing
				if (m_events.window.event == SDL_WINDOWEVENT_RESIZED)
					Application::window()->m_changed = true;
		}
	}
}

int Input::m_keyCount;
const uint8* Input::m_keyboardState = SDL_GetKeyboardState(&Input::m_keyCount);
uint8* Input::m_prevKeyboardState = new uint8[Input::m_keyCount];

glm::ivec2 Input::m_mousePos;

uint32 Input::m_mouseState = SDL_GetMouseState(&Input::m_mousePos.x, &Input::m_mousePos.y);
uint32 Input::m_prevMouseState = m_mouseState;

SDL_Event Input::m_events;

} // namespace lyra
