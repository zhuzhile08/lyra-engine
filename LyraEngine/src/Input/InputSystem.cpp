#include <Input/InputSystem.h>

#include <Common/Logger.h>

#include <Graphics/SDLWindow.h>

#include <SDL3/SDL.h>
#include <backends/imgui_impl_sdl3.h>

#include <Common/UnorderedSparseMap.h>

namespace lyra {

namespace input {

namespace {

class InputSystem {
public:
	InputSystem() noexcept = default;
	InputSystem(Window& window, const ImGuiContext* context) noexcept : window(&window), imGUI(context) { 
		mouseState = SDL_GetMouseState(&mousePos.x, &mousePos.y);
		keyboardState = SDL_GetKeyboardState(nullptr);
	}

	UnorderedSparseMap<input::KeyType, input::Key> keys;
	UnorderedSparseMap<input::MouseButtonType, input::MouseButton> mouseButtons;
	UnorderedSparseMap<input::ControllerButtonType, input::ControllerButton> controllerButtons;

	uint32 mouseState;
	const uint8* keyboardState;

	glm::vec2 mousePos;
	glm::vec2 mouseDelta;
	glm::vec2 stickPos;
	glm::vec2 stickDelta;

	Window* window;
	const ImGuiContext* imGUI = nullptr;
};

static InputSystem* globalInputSystem = nullptr;

}

const Key& keyboard(KeyType type) noexcept {
	return globalInputSystem->keys[type];
}
const MouseButton& mouse(MouseButtonType type) noexcept {
	return globalInputSystem->mouseButtons[type];
}
const ControllerButton& controller(ControllerButtonType type) noexcept {
	return globalInputSystem->controllerButtons[type];
}
const glm::vec2& mousePos() noexcept {
	return globalInputSystem->mousePos;
}
const glm::vec2& mouseDelta() noexcept {
	return globalInputSystem->mouseDelta;
}
const glm::vec2& analogueStickPos() noexcept {
	return globalInputSystem->stickPos;
}

void update() {
	globalInputSystem->window->m_changed = false;
	
	SDL_PumpEvents();
	
	for (auto& button : globalInputSystem->controllerButtons) {
		button.second.reset();
	}
	
	globalInputSystem->mouseDelta = -globalInputSystem->mousePos;
	auto mState = SDL_GetMouseState(&globalInputSystem->mousePos.x, &globalInputSystem->mousePos.y);
	globalInputSystem->mouseDelta += globalInputSystem->mousePos;
	for (auto& button : globalInputSystem->mouseButtons) {
		button.second.reset();
		
		if ((globalInputSystem->mouseState & static_cast<uint32>(button.first)) == static_cast<uint32>(button.first)) button.second.held = true;
		std::swap(mState, globalInputSystem->mouseState);
	}

	auto kState = SDL_GetKeyboardState(nullptr);
	for (auto& key : globalInputSystem->keys) {
		key.second.reset();
		
		if (globalInputSystem->keyboardState[static_cast<size_type>(key.first)] && kState[static_cast<size_type>(key.first)]) key.second.held = true;
		std::swap(kState, globalInputSystem->keyboardState);
	}

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (globalInputSystem->imGUI) ImGui_ImplSDL3_ProcessEvent(&event);
		
		// implement controller events @todo

		// default events that are always checked
		switch (event.type) {
			case SDL_EVENT_QUIT:
				globalInputSystem->window->m_running = false;
			case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
			case SDL_EVENT_WINDOW_MINIMIZED:
			case SDL_EVENT_WINDOW_MAXIMIZED:
				globalInputSystem->window->m_changed = true;
			case SDL_EVENT_KEY_DOWN:
				for (auto& key : globalInputSystem->keys) {
					if (event.key.keysym.sym == static_cast<int>(key.first)) key.second.pressed = true;
				}
			case SDL_EVENT_KEY_UP:
				for (auto& key : globalInputSystem->keys) {
					if (event.key.keysym.sym == static_cast<int>(key.first)) key.second.released = true;
				}
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				for (auto& key : globalInputSystem->mouseButtons) {
					if (event.button.button == static_cast<uint8>(key.first)) key.second.pressed = true;
				}
			case SDL_EVENT_MOUSE_BUTTON_UP:
				for (auto& key : globalInputSystem->mouseButtons) {
					if (event.button.button == static_cast<uint8>(key.first)) key.second.released = true;
				}
			case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
				for (auto& key : globalInputSystem->controllerButtons) {
					if (event.button.button == static_cast<uint8>(key.first)) key.second.pressed = true;
				}
			case SDL_EVENT_GAMEPAD_BUTTON_UP:
				for (auto& key : globalInputSystem->controllerButtons) {
					if (event.button.button == static_cast<uint8>(key.first)) key.second.released = true;
				}
		}
	}
}

void enableImGui(const ImGuiContext* context) noexcept {
	globalInputSystem->imGUI = context;
}
void disableImGui() noexcept {
	globalInputSystem->imGUI = nullptr;
}

} // namespace input

void initInputSystem(Window& window, const ImGuiContext* context) {
	if (input::globalInputSystem) {
		log::error("lyra::initInputSystem(): The input system is already initialized!");
		return;
	}

	input::globalInputSystem = new input::InputSystem(window, context);
}

} // namespace lyra
