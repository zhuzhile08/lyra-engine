/*************************
 * @file Input.h
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

#include <SDL.h>
#include <glm/glm.hpp>

#include <unordered_map>

namespace lyra {

void init_input_system(Window& window);

namespace input {

class Button {
public:
	bool pressed;
	bool released;
	bool held;

	friend class Input;
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

class Input {
public:
	Input() noexcept = default;
	Input(Window& window) noexcept : m_window(&window) { }

	void add_keyboard_input(KeyType type) {
		m_keys.insert({type, Key()});
	}
	void add_mouse_button_input(MouseButtonType type) {
		m_mouseButtons.insert({type, MouseButton()});
	}
	void add_controller_button_input(ControllerButtonType type) {
		m_controllerButtons.insert({type, ControllerButton()});
	}

	const Key& keyboard_input(KeyType type) const noexcept {
		return m_keys.at(type);
	}
	const MouseButton& mouse_input(MouseButtonType type) const noexcept {
		return m_mouseButtons.at(type);
	}
	const ControllerButton& controller_input(ControllerButtonType type) const noexcept {
		return m_controllerButtons.at(type);
	}

	const glm::ivec2& mouse_pos() const noexcept {
		return m_mousePos;
	}
	const glm::vec2& analogue_stick_pos() const noexcept {
		return m_stickPos;
	}

	void update();

private:
	std::unordered_map<KeyType, Key> m_keys;
	std::unordered_map<MouseButtonType, MouseButton> m_mouseButtons;
	std::unordered_map<ControllerButtonType, ControllerButton> m_controllerButtons;

	uint32 m_mouseState;
	const uint8* m_keyboardState;

	glm::ivec2 m_mousePos;
	glm::vec2 m_stickPos;

	Window* m_window;
};

namespace detail {

Input* const global_input_system();

} // namespace detail

inline void add_keyboard_input(KeyType type) {
	detail::global_input_system()->add_keyboard_input(type);
}
inline void add_mouse_button_input(MouseButtonType type) {
	detail::global_input_system()->add_mouse_button_input(type);
}
inline void add_controller_button_input(ControllerButtonType type) {
	detail::global_input_system()->add_controller_button_input(type);
}
inline const Key& keyboard_input(KeyType type) noexcept {
	return detail::global_input_system()->keyboard_input(type);
}
inline const MouseButton& mouse_input(MouseButtonType type) noexcept {
	return detail::global_input_system()->mouse_input(type);
}
inline const ControllerButton& controller_input(ControllerButtonType type) noexcept {
	return detail::global_input_system()->controller_input(type);
}
inline const glm::ivec2& mouse_pos() noexcept {
	return detail::global_input_system()->mouse_pos();
}
inline const glm::vec2& analogue_stick_pos() noexcept {
	return detail::global_input_system()->analogue_stick_pos();
}
inline void update() {
	detail::global_input_system()->update();
}

} // namespace input

} // namespace lyra
