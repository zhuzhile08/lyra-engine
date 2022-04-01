#define SDL_MAIN_HANDLED
#define GLM_FORCE_RADIANS

#include <core/logger.h>
#include <core/queue_types.h>
#include <graphics/window.h>
#include <core/rendering/context.h>
#include <graphics/renderer.h>
#include <graphics/mesh.h>
#include <graphics/texture.h>
#include <init/init_SDL.h>

int main() {
	lyra::init_SDL();
	lyra::Window window;

	lyra::Context context;
	context.create(&window);

	lyra::Mesh room;
	room.create(&context, lyra::load_model("data/model/viking_room.obj"));

	lyra::Texture texture;
	texture.create(&context, "data/img/viking_room.png");

	lyra::Renderer renderer;
	renderer.create(&context);

	room.draw(renderer);
	texture.draw(renderer);

	renderer.draw();

	while (true) {
		context.draw();
	}

	return 0;
}
