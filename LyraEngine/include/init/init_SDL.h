/*************************
 * @file init_SDL.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief initialize of quit the SDL2 libary (https://www.libsdl.org/download-2.0.php)
 * 
 * @date 2022-02-03
 * 
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <SDL.h>

#include <core/defines.h>

#include <core/logger.h>
#include <SDL_error.h>

namespace lyra {

/**
 * @brief initialize SDL
 */
void init_SDL();

/**
 * @brief quit SDL
 */
void quit_SDL();

} // namespace lyra
