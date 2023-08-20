#include <Input/Input.h>

#include <backends/imgui_impl_sdl2.h>

namespace lyra {

namespace input {

namespace detail {

static Input* globalInputSystem = nullptr;

Input* const defaultInputSystem() {
	ASSERT(globalInputSystem, "The Input system was not initialized before usage!");
	return globalInputSystem;
}

}

void Input::update() {
	m_window->m_changed = false;

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		// ImGui_ImplSDL2_ProcessEvent(&m_events);

		// get new events and set the passed events
		SDL_PumpEvents();

		m_mouseState = SDL_GetMouseState(&m_mousePos.x, &m_mousePos.y);
		for (auto& button : m_mouseButtons) {
			if ((m_mouseState & static_cast<uint32>(button.first)) == static_cast<uint32>(button.first)) button.second.held = true;
		}

		m_keyboardState = SDL_GetKeyboardState(nullptr);
		for (auto& key : m_keys) {
			if (m_keyboardState[static_cast<size_t>(key.first)]) key.second.held = true;
		}

		// implement controller events @todo

		// default events that are always checked
		switch (event.type) {
			case SDL_QUIT:
				m_window->m_running = false;
			case SDL_WINDOWEVENT: 
				if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED || 
					event.window.event == SDL_WINDOWEVENT_MINIMIZED || 
					event.window.event == SDL_WINDOWEVENT_MAXIMIZED)
					m_window->m_changed = true;
			case SDL_KEYDOWN:
				for (auto& key : m_keys) {
					if (event.key.keysym.sym == static_cast<int>(key.first)) key.second.pressed = true;
				}
			case SDL_KEYUP:
				for (auto& key : m_keys) {
					if (event.key.keysym.sym == static_cast<int>(key.first)) key.second.released = true;
				}
			case SDL_MOUSEBUTTONDOWN:
				for (auto& key : m_mouseButtons) {
					if (event.button.button == static_cast<uint8>(key.first)) key.second.pressed = true;
				}
			case SDL_MOUSEBUTTONUP:
				for (auto& key : m_mouseButtons) {
					if (event.button.button == static_cast<uint8>(key.first)) key.second.released = true;
				}
			case SDL_CONTROLLERBUTTONDOWN:
				for (auto& key : m_controllerButtons) {
					if (event.cbutton.button == static_cast<uint8>(key.first)) key.second.pressed = true;
				}
			case SDL_CONTROLLERBUTTONUP:
				for (auto& key : m_controllerButtons) {
					if (event.cbutton.button == static_cast<uint8>(key.first)) key.second.released = true;
				}
					
		}
	}
}

} // namespace input

void initInputSystem(Window& window) {
	input::detail::globalInputSystem = new input::Input(window);
}

} // namespace lyra
