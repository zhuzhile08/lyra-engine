#include <init/init_SDL.h>

namespace lyra {

void init_SDL() {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {       // I'm way too lazy to specify everything that needs to be initialized. It's a game engine, so you'd need everything, right?
		LOG_EXEPTION("SDL init", SDL_GetError());
	}
}

void quit_SDL() {
	SDL_Quit();
}

}
