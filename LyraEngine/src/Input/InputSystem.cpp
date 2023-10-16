#include <Input/InputSystem.h>

#include <backends/imgui_impl_sdl3.h>

namespace lyra {

static InputSystem* globalInputSystem = nullptr;

void initInputSystem(Window& window, const ImGuiContext* context) {
	if (globalInputSystem) {
		log::error("lyra::initInputSystem(): The input system is already initialized!");
		return;
	}

	globalInputSystem = new InputSystem(window, context);
}

namespace input {

namespace detail {

} // namespace detail

void addKeyboardInput(KeyType type) {
	globalInputSystem->addKeyboardInput(type);
}
void addMouseButtonInput(MouseButtonType type) {
	globalInputSystem->addMouseButtonInput(type);
}
void addControllerButtonInput(ControllerButtonType type) {
	globalInputSystem->addControllerButtonInput(type);
}
const Key& keyboardInput(KeyType type) noexcept {
	return globalInputSystem->keyboardInput(type);
}
const MouseButton& mouseInput(MouseButtonType type) noexcept {
	return globalInputSystem->mouseInput(type);
}
const ControllerButton& controllerInput(ControllerButtonType type) noexcept {
	return globalInputSystem->controllerInput(type);
}
const glm::vec2& mousePos() noexcept {
	return globalInputSystem->mousePos();
}
const glm::vec2& analogueStickPos() noexcept {
	return globalInputSystem->analogueStickPos();
}
void update() {
	globalInputSystem->update();
}
void enableImGui(const ImGuiContext* context) noexcept {
	globalInputSystem->enableImGui(context);
}
void disableImGui() noexcept {
	globalInputSystem->disableImGui();
}

} // namespace input

void InputSystem::update() {
	m_window->m_changed = false;

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (m_imGUI) ImGui_ImplSDL3_ProcessEvent(&event);
		
		// implement controller events @todo

		// default events that are always checked
		switch (event.type) {
			case SDL_EVENT_QUIT:
				m_window->m_running = false;
			case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED: 
			case SDL_EVENT_WINDOW_MINIMIZED:
			case SDL_EVENT_WINDOW_MAXIMIZED:
					m_window->m_changed = true;
			case SDL_EVENT_KEY_DOWN:
				for (auto& key : m_keys) {
					if (event.key.keysym.sym == static_cast<int>(key.first)) key.second.pressed = true;
				}
			case SDL_EVENT_KEY_UP:
				for (auto& key : m_keys) {
					if (event.key.keysym.sym == static_cast<int>(key.first)) key.second.released = true;
				}
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				for (auto& key : m_mouseButtons) {
					if (event.button.button == static_cast<uint8>(key.first)) key.second.pressed = true;
				}
			case SDL_EVENT_MOUSE_BUTTON_UP:
				for (auto& key : m_mouseButtons) {
					if (event.button.button == static_cast<uint8>(key.first)) key.second.released = true;
				}
			case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
				for (auto& key : m_controllerButtons) {
					if (event.button.button == static_cast<uint8>(key.first)) key.second.pressed = true;
				}
			case SDL_EVENT_GAMEPAD_BUTTON_UP:
				for (auto& key : m_controllerButtons) {
					if (event.button.button == static_cast<uint8>(key.first)) key.second.released = true;
				}	
		}
	}

	m_mouseState = SDL_GetMouseState(&m_mousePos.x, &m_mousePos.y);
	for (auto& button : m_mouseButtons) {
		if ((m_mouseState & static_cast<uint32>(button.first)) == static_cast<uint32>(button.first)) button.second.held = true;
	}

	m_keyboardState = SDL_GetKeyboardState(nullptr);
	for (auto& key : m_keys) {
		if (m_keyboardState[static_cast<size_t>(key.first)]) key.second.held = true;
	}
}

} // namespace lyra
