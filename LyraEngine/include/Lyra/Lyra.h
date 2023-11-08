/*************************
 * @file Lyra.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief The Engine header including all other headers as well as some initialization functions
 * 
 * @date 2023-06-24
 * 
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#ifdef _WIN32
#define SDL_MAIN_HANDLED
#endif

#include <SDL_main.h>

#include <Graphics/Renderer.h>
#include <Graphics/SDLWindow.h>

namespace lyra {

inline void init() {
	if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS) != 0) {
		ASSERT(false, "lyra::init(): SDL init error: {}!", SDL_GetError());
	}
}

inline void quit() {
	quitRenderSystem();
	SDL_Quit();
}

} // namespace lyra