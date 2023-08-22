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
	Input(Window& window, const ImGuiContext* context) noexcept : m_window(&window), m_imGUI(context) { }

	void addKeyboardInput(KeyType type) {
		m_keys.insert({type, Key()});
	}
	void addMouseButtonInput(MouseButtonType type) {
		m_mouseButtons.insert({type, MouseButton()});
	}
	void addControllerButtonInput(ControllerButtonType type) {
		m_controllerButtons.insert({type, ControllerButton()});
	}

	const Key& keyboardInput(KeyType type) const noexcept {
		return m_keys.at(type);
	}
	const MouseButton& mouseInput(MouseButtonType type) const noexcept {
		return m_mouseButtons.at(type);
	}
	const ControllerButton& controllerInput(ControllerButtonType type) const noexcept {
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
	std::unordered_map<KeyType, Key> m_keys;
	std::unordered_map<MouseButtonType, MouseButton> m_mouseButtons;
	std::unordered_map<ControllerButtonType, ControllerButton> m_controllerButtons;

	uint32 m_mouseState;
	const uint8* m_keyboardState;

	glm::ivec2 m_mousePos;
	glm::vec2 mStickPos;

	Window* m_window;
	const ImGuiContext* m_imGUI = nullptr;
};

namespace detail {

Input* const defaultInputSystem();

} // namespace detail

inline void addKeyboardInput(KeyType type) {
	detail::defaultInputSystem()->addKeyboardInput(type);
}
inline void addMouseButtonInput(MouseButtonType type) {
	detail::defaultInputSystem()->addMouseButtonInput(type);
}
inline void addControllerButtonInput(ControllerButtonType type) {
	detail::defaultInputSystem()->addControllerButtonInput(type);
}
inline const Key& keyboardInput(KeyType type) noexcept {
	return detail::defaultInputSystem()->keyboardInput(type);
}
inline const MouseButton& mouseInput(MouseButtonType type) noexcept {
	return detail::defaultInputSystem()->mouseInput(type);
}
inline const ControllerButton& controllerInput(ControllerButtonType type) noexcept {
	return detail::defaultInputSystem()->controllerInput(type);
}
inline const glm::ivec2& mousePos() noexcept {
	return detail::defaultInputSystem()->mousePos();
}
inline const glm::vec2& analogueStickPos() noexcept {
	return detail::defaultInputSystem()->analogueStickPos();
}
inline void update() {
	detail::defaultInputSystem()->update();
}
inline void enableImGui(const ImGuiContext* context) noexcept {
	detail::defaultInputSystem()->enableImGui(context);
}
inline void disableImGui() noexcept {
	detail::defaultInputSystem()->disableImGui();
}

} // namespace input

} // namespace lyra
