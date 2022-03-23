#define SDL_MAIN_HANDLED
#define GLM_FORCE_RADIANS

#include <core/logger.h>
#include <core/queue_types.h>
#include <rendering/window.h>
#include <rendering/renderer.h>
#include <rendering/render_stage.h>
#include <res/mesh.h>
#include <res/texture.h>
#include <init/init_SDL.h>

int main() {
	lyra::init_SDL();
	lyra::Window window;

	lyra::Renderer renderer;
	renderer.create(window);

	lyra::Mesh room;
	room.create(renderer, lyra::load_model("data/model/viking_room.obj"));

	lyra::Texture texture;
	texture.create(renderer, "data/img/viking_room.png");

	lyra::RenderStage stage;
	stage.create(renderer);

	room.draw(stage);
	texture.draw(stage);

	stage.draw();

	renderer.update();

	return 0;
}
