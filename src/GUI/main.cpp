#define SDL_MAIN_HANDLED
#define GLM_FORCE_RADIANS

#include <core/logger.h>
#include <rendering/window.h>
#include <rendering/renderer.h>
#include <rendering/render_stage.h>
#include <res/mesh.h>
#include <init/init_SDL.h>

int main() {
	lyra::init_SDL();
	lyra::Window window;

	lyra::Renderer renderer;
	renderer.create(window);

	lyra::Mesh room;
	room.create(renderer, lyra::load_model("data/model/viking_room.obj"));

	/** 
	lyra::Texture texture;
	texture.create(renderer, "data/img/viking_room.png");
	 */

	lyra::RenderStage stage;
	lyra::VulkanDescriptor::Writer writer;
	/** 
	writer.add_image_write();
	 */
	stage.create(renderer, 2, writer);

	room.draw(stage);
	stage.draw(renderer);

	renderer.update();

	return 0;
}
