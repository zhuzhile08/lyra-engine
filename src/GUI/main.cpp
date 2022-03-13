#define SDL_MAIN_HANDLED
#define GLM_FORCE_RADIANS

#include <core/logger.h>
#include <rendering/window.h>
#include <rendering/renderer.h>
#include <init/init_SDL.h>

int main() {
	lyra::init_SDL();
	lyra::Window window;
	lyra::Renderer renderer;
	renderer.create(window);

	return 0;
}
