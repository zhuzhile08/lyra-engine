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

#include <LSD/Hash.h>

#include <glm/glm.hpp>
#include <imgui.h>

namespace lyra {

void initInputSystem(const ImGuiContext* context = nullptr);

namespace input {

class Button {
public:
	bool pressed;
	bool released;
	bool held;
	
	void reset() noexcept {
		pressed = false;
		released = false;
		held = false;
	}
};

class Key : public Button {
public:
	Key() noexcept = default;
	Key(KeyType t) noexcept : type(t) { }
	
	KeyType type;
};

class MouseButton : public Button {
public:
	MouseButton() noexcept = default;
	MouseButton(MouseButtonType t) noexcept : type(t) { }
	
	MouseButtonType type;
};

class ControllerButton : public Button {
public:
	ControllerButton() noexcept = default;
	ControllerButton(ControllerButtonType t) noexcept : type(t) { }
	
	ControllerButtonType type;
};


bool quit();
void cancelQuit();

const Key& keyboard(KeyType type);
const MouseButton& mouse(MouseButtonType type);
const ControllerButton& controller(ControllerButtonType type);
const glm::vec2& mousePos();
const glm::vec2& mouseDelta();
const glm::vec2& analogueStickPos();

void update();
void enableImGui(const ImGuiContext* context);
void disableImGui();

} // namespace input

} // namespace lyra

namespace lsd {

template <> struct Hash<lyra::input::Key> {
	constexpr std::size_t operator()(const lyra::input::Key& k) const noexcept {
		return static_cast<std::size_t>(k.type);
	}
};

template <> struct Hash<lyra::input::MouseButton> {
	constexpr std::size_t operator()(const lyra::input::MouseButton& b) const noexcept {
		return static_cast<std::size_t>(b.type);
	}
};

template <> struct Hash<lyra::input::ControllerButton> {
	constexpr std::size_t operator()(const lyra::input::ControllerButton& b) const noexcept {
		return static_cast<std::size_t>(b.type);
	}
};

} // namespace lsd
