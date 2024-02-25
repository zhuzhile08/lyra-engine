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

#include <Input/InputEnums.h>

#include <glm/glm.hpp>
#include <imgui.h>

namespace lyra {

void initInputSystem(Window& window, const ImGuiContext* context = nullptr);

namespace input {

class Button {
public:
	bool pressed;
	bool released;
	bool held;
	
	void reset() {
		pressed = false;
		released = false;
		held = false;
	}

	friend class InputSystem;
};

class Key : public Button {
public:
	Key() = default;
	Key(KeyType t) : type(t) { }
	
	KeyType type;
};

class MouseButton : public Button {
public:
	MouseButton() = default;
	MouseButton(MouseButtonType t) : type(t) { }
	
	MouseButtonType type;
};

class ControllerButton : public Button {
public:
	ControllerButton() = default;
	ControllerButton(ControllerButtonType t) : type(t) { }
	
	ControllerButtonType type;
};


const Key& keyboard(KeyType type) noexcept;
const MouseButton& mouse(MouseButtonType type) noexcept;
const ControllerButton& controller(ControllerButtonType type) noexcept;
const glm::vec2& mousePos() noexcept;
const glm::vec2& mouseDelta() noexcept;
const glm::vec2& analogueStickPos() noexcept;
void update();
void enableImGui(const ImGuiContext* context) noexcept;
void disableImGui() noexcept;

} // namespace input

} // namespace lyra

namespace std {

template <> struct hash<lyra::input::Key> {
	size_t operator()(const lyra::input::Key& k) const {
		return static_cast<size_t>(k.type);
	}
};

template <> struct hash<lyra::input::MouseButton> {
	size_t operator()(const lyra::input::MouseButton& b) const {
		return static_cast<size_t>(b.type);
	}
};

template <> struct hash<lyra::input::ControllerButton> {
	size_t operator()(const lyra::input::ControllerButton& b) const {
		return static_cast<size_t>(b.type);
	}
};

} // namespace std
