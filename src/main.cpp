#define SDL_MAIN_HANDLED
#define GLM_FORCE_RADIANS

#include <lyra.h>
#include <core/logger.h>
#include <core/queue_types.h>
#include <graphics/renderer.h>
#include <components/mesh/mesh.h>
#include <graphics/texture.h>
#include <math/math.h>
#include <glm.hpp>

int main() {
	lyra::Application::init();

	lyra::Renderer renderer;

	lyra::Texture texture;
	texture.create({ "data/img/viking_room.png" });

	lyra::Camera camera;
	camera.set_rotation(90.0f, { 0.0f, 0.0f, 1.0f });
	camera.look_at({ 0.0f, 0.0f, 0.0f });
	camera.set_perspective(camera.aspect());

	lyra::Mesh room;
	room.bind_camera(&camera);
	room.bind_texture(&texture);
	room.create("data/model/viking_room.obj");
	room.bind(&renderer);

	renderer.draw();

	while (true) {
		camera.draw();
		lyra::Application::draw();
	}

	return 0;
}
