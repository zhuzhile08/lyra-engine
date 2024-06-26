#include <Input/InputSystem.h>

#include <Common/Logger.h>

#include <Graphics/Window.h>

#include <SDL3/SDL.h>
#include <backends/imgui_impl_sdl3.h>

#include <LSD/UnorderedSparseMap.h>

namespace lyra {

namespace renderer {

extern Window* globalWindow;

} // namespace renderer

namespace input {

namespace {

class InputSystem {
public:
	InputSystem() noexcept = default;
	InputSystem(const ImGuiContext* context) noexcept : imGUI(context) { 
		mouseState = SDL_GetMouseState(&mousePos.x, &mousePos.y);
		keyboardState = SDL_GetKeyboardState(nullptr);
	}

	lsd::UnorderedSparseMap<input::KeyType, input::Key> keys;
	lsd::UnorderedSparseMap<input::MouseButtonType, input::MouseButton> mouseButtons;
	lsd::UnorderedSparseMap<input::ControllerButtonType, input::ControllerButton> controllerButtons;

	bool quit = false;

	uint32 mouseState;
	const uint8* keyboardState;

	glm::vec2 mousePos;
	glm::vec2 mouseDelta;
	glm::vec2 stickPos;
	glm::vec2 stickDelta;

	const ImGuiContext* imGUI = nullptr;
};

}

static InputSystem* globalInputSystem = nullptr;

bool quit() {
	return globalInputSystem->quit;
}
void cancelQuit() {
	globalInputSystem->quit = false;
}

const Key& keyboard(KeyType type) {
	return globalInputSystem->keys[type];
}
const MouseButton& mouse(MouseButtonType type) {
	return globalInputSystem->mouseButtons[type];
}
const ControllerButton& controller(ControllerButtonType type) {
	return globalInputSystem->controllerButtons[type];
}
const glm::vec2& mousePos() {
	return globalInputSystem->mousePos;
}
const glm::vec2& mouseDelta() {
	return globalInputSystem->mouseDelta;
}
const glm::vec2& analogueStickPos() {
	return globalInputSystem->stickPos;
}

void update() {
	renderer::globalWindow->changed = false;
	globalInputSystem->mouseDelta = { 0, 0 };
	
	SDL_PumpEvents();
	
	for (auto& button : globalInputSystem->controllerButtons) {
		button.second.reset();
	}
	
	auto mState = SDL_GetMouseState(&globalInputSystem->mousePos.x, &globalInputSystem->mousePos.y);
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
				globalInputSystem->quit = true;
			case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
			case SDL_EVENT_WINDOW_MINIMIZED:
			case SDL_EVENT_WINDOW_MAXIMIZED:
				renderer::globalWindow->changed = true;
			case SDL_EVENT_KEY_DOWN:
				for (auto& key : globalInputSystem->keys) {
					if (event.key.key == static_cast<uint>(key.first)) key.second.pressed = true;
				}
			case SDL_EVENT_KEY_UP:
				for (auto& key : globalInputSystem->keys) {
					if (event.key.key == static_cast<uint>(key.first)) key.second.released = true;
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

			case SDL_EVENT_MOUSE_MOTION:
				globalInputSystem->mouseDelta = { event.motion.xrel, event.motion.yrel };
		}
	}
}

void enableImGui(const ImGuiContext* context) {
	globalInputSystem->imGUI = context;
}
void disableImGui() {
	globalInputSystem->imGUI = nullptr;
}

} // namespace input

void initInputSystem(const ImGuiContext* context) {
	if (input::globalInputSystem) 
		log::error("lyra::initInputSystem(): The input system is already initialized!");
	else 
		input::globalInputSystem = new input::InputSystem(context);
}

} // namespace lyra
