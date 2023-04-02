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

#include <Lyra/Lyra.h>

#include <SDL.h>
#include <glm/glm.hpp>

#include <Common/Queue.h>

#include <Input/InputEnums.h>

namespace lyra {

namespace input {

class InputManager {
public:
	/**
	 * @brief check if a key has been pressed on the keyboard
	 * 
	 * @param event the key to check for
	 * 
	 * @return true 
	 * @return false 
	 */
	NODISCARD static bool is_key_held(const Keyboard& event) {
		return m_keyboardState[static_cast<SDL_Scancode>(event)];
	}
	/**
	 * @brief check if a mouse button was pressed
	 * 
	 * @param mouse mouse event to check for
	 * 
	 * @return true 
	 * @return false 
	 */
	NODISCARD static bool is_mouse_button_held(const Mouse& mouse) {
		return (m_mouseState & static_cast<uint8>(mouse)) != 0;
	}

	/**
	 * @brief get the mouse position
	 * 
	 * @return lyra::glm::ivec2
	 */
	NODISCARD static constexpr glm::ivec2 get_mouse_position() { return m_mousePos; }
	/**
	 * @brief get the mouse position
	 * 
	 * @param mousePos empty mouse position vector
	 */
	DEPRECATED static void get_mouse_position(glm::ivec2& mousePos) {
		mousePos = m_mousePos;
	}

	/**
	 * @brief update the keys. Called every frame
	 */
	static void update();
	/**
	 * @brief wait until an event was detected
	 */
	static void wait_events() { SDL_WaitEvent(&m_events); }

private:
	static int m_keyCount;
	static const uint8* m_keyboardState;

	static uint32 m_mouseState;
	static glm::ivec2 m_mousePos;

	static SDL_Event m_events;
};

}

} // namespace lyra
