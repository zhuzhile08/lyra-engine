#include <init/init_SDL.h>

#include <SDL.h>

#include <core/logger.h>
#include <SDL_error.h>

namespace lyra {

void init_SDL() {
	if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_EVENTS) != 0) {
		Logger::log_exception("SDL init error: ", SDL_GetError());
	}
}

void quit_SDL() {
	SDL_Quit();
}

}
