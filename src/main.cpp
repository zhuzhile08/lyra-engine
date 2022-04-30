#define SDL_MAIN_HANDLED
#define GLM_FORCE_RADIANS

#include <core/logger.h>
#include <core/queue_types.h>
#include <core/rendering/window.h>
#include <core/rendering/context.h>
#include <graphics/renderer.h>
#include <graphics/mesh.h>
#include <graphics/texture.h>
#include <init/init_SDL.h>
#include <math/math.h>
#include <core/rendering/gui_context.h>
#include <glm.hpp>

int main() {
	lyra::init_SDL();
	lyra::Window window;

	lyra::Context context;
	context.create(&window);

	lyra::Renderer renderer;
	renderer.create(&context);

	lyra::gui::GUIContext guiContext;
	guiContext.create(&context, &renderer, &window);

	lyra::Texture texture;
	texture.create(&context, "data/img/viking_room.png");

	lyra::Camera camera;
	camera.create(&context);
	camera.set_rotation(90.0f, { 0.0f, 0.0f, 1.0f });
	camera.look_at({ 0.0f, 0.0f, 0.0f });
	camera.set_perspective(camera.aspect());

	lyra::Mesh room;
	room.bind_camera(&camera);
	room.bind_texture(&texture);
	room.create(&context, lyra::load_model("data/model/viking_room.obj"));
	room.bind(&renderer);

	renderer.draw();

	while (true) {
		camera.draw();
		context.draw();
	}

	return 0;
}
