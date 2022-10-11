#include <input/input.h>

#include <backends/imgui_impl_sdl.h>

namespace lyra {

const bool Input::check_key(Keyboard event) {
	if (m_keyboardState[static_cast<SDL_Scancode>(event)]) return true;
	else return false;
}

const bool Input::check_key_down(Keycode event) {
	if (m_events.type == SDL_KEYDOWN && m_events.key.keysym.sym == static_cast<SDL_Keycode>(event)) return true;
	else return false;
}

const bool Input::check_key_up(Keycode event) {
	if (m_events.type == SDL_KEYUP && m_events.key.keysym.sym == static_cast<SDL_Keycode>(event)) return true;
	else return false;
}

const bool Input::check_mouse_button(MouseMask mouse) {
	uint8 mouseEvents = SDL_GetMouseState(nullptr, nullptr);
	if ((mouseEvents & static_cast<uint8>(mouse)) != 0) return true;
	else return false;
}

const bool Input::check_mouse_button_down(Mouse mouse) {
	if (m_events.type == SDL_MOUSEBUTTONDOWN && m_events.button.button == static_cast<uint8>(mouse)) return true;
	else return false;
}

const bool Input::check_mouse_button_up(Mouse mouse) {
	if (m_events.type == SDL_MOUSEBUTTONUP && m_events.button.button == static_cast<uint8>(mouse)) return true;
	else return false;
}

const glm::vec2 Input::check_mouse_position() {
	int x, y;
	SDL_GetMouseState(&x, &y);
	glm::vec2 mousePos = {x, y};
	return mousePos;
}

void Input::check_mouse_position(glm::vec2& mousePos) {
	int x, y;
	SDL_GetMouseState(&x, &y);
	mousePos = {x, y};
}

void Input::update() {
	SDL_PumpEvents();

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

const uint8* Input::m_keyboardState = SDL_GetKeyboardState(nullptr);
SDL_Event Input::m_events;

} // namespace lyra
