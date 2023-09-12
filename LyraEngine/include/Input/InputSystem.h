/*************************
 * @file InputSystem.h
 * @author zhuzhile08 (zhuzhile08@gmail.com)
 * 
 * @brief A singleton wrapper around SDL input which polls events
 * 
 * @date 2022-10-11
 * 
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <Common/Common.h>

#include <Graphics/SDLWindow.h>

#include <Input/InputEnums.h>

#include <glm/glm.hpp>
#include <SDL.h>
#include <imgui.h>

#include <unordered_map>

namespace lyra {

void initInputSystem(Window& window, const ImGuiContext* context = nullptr);

namespace input {

class Button {
public:
	bool pressed;
	bool released;
	bool held;

	friend class InputSystem;
};

class Key : public Button {
public:
	KeyType type;
};

class MouseButton : public Button {
public:
	MouseButtonType type;
};

class ControllerButton : public Button {
public:
	ControllerButtonType type;
};


void addKeyboardInput(KeyType type);
void addMouseButtonInput(MouseButtonType type);
void addControllerButtonInput(ControllerButtonType type);
const Key& keyboardInput(KeyType type) noexcept;
const MouseButton& mouseInput(MouseButtonType type) noexcept;
const ControllerButton& controllerInput(ControllerButtonType type) noexcept;
const glm::ivec2& mousePos() noexcept;
const glm::vec2& analogueStickPos() noexcept;
void update();
void enableImGui(const ImGuiContext* context) noexcept;
void disableImGui() noexcept;

}

class InputSystem {
public:
	InputSystem() noexcept = default;
	InputSystem(Window& window, const ImGuiContext* context) noexcept : m_window(&window), m_imGUI(context) { }

	void addKeyboardInput(input::KeyType type) {
		m_keys.insert({type, input::Key()});
	}
	void addMouseButtonInput(input::MouseButtonType type) {
		m_mouseButtons.insert({type, input::MouseButton()});
	}
	void addControllerButtonInput(input::ControllerButtonType type) {
		m_controllerButtons.insert({type, input::ControllerButton()});
	}

	const input::Key& keyboardInput(input::KeyType type) const noexcept {
		return m_keys.at(type);
	}
	const input::MouseButton& mouseInput(input::MouseButtonType type) const noexcept {
		return m_mouseButtons.at(type);
	}
	const input::ControllerButton& controllerInput(input::ControllerButtonType type) const noexcept {
		return m_controllerButtons.at(type);
	}

	const glm::ivec2& mousePos() const noexcept {
		return m_mousePos;
	}
	const glm::vec2& analogueStickPos() const noexcept {
		return mStickPos;
	}

	constexpr void enableImGui(const ImGuiContext* context) noexcept {
		m_imGUI = context;
	}
	constexpr void disableImGui() noexcept {
		m_imGUI = nullptr;
	}

	void update();

private:
	std::unordered_map<input::KeyType, input::Key> m_keys;
	std::unordered_map<input::MouseButtonType, input::MouseButton> m_mouseButtons;
	std::unordered_map<input::ControllerButtonType, input::ControllerButton> m_controllerButtons;

	uint32 m_mouseState;
	const uint8* m_keyboardState;

	glm::ivec2 m_mousePos;
	glm::vec2 mStickPos;

	Window* m_window;
	const ImGuiContext* m_imGUI = nullptr;
};

} // namespace lyra
