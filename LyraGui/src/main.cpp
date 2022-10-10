#define SDL_MAIN_HANDLED
#define LYRA_LOG_FILE

#include <Jolt/Jolt.h>
#include <core/logger.h>
#include <core/application.h>
#include <core/rendering/material.h>
#include <core/rendering/vulkan/descriptor.h>
#include <core/rendering/vulkan/vulkan_window.h>
#include <core/rendering/graphics_pipeline.h>
#include <core/rendering/vulkan/vulkan_shader.h>
#include <core/rendering/vulkan/vulkan_pipeline.h>
#include <core/rendering/gui_renderer.h>
#include <nodes/mesh/mesh.h>
#include <nodes/mesh/mesh_renderer.h>
#include <nodes/graphics/camera.h>
#include <nodes/spatial.h>
#include <core/rendering/texture.h>
#include <imgui.h>

#include <array>
/** 
class Camera : public lyra::Node, public lyra::Camera {
public:
	Camera() { init(); }

	void update(void) override {
		CameraData data;
		rotate({ 0.0f, 0.0f, lyra::Application::fps() * 90.0f });
		look_at({ 0.0f, 0.0f, 0.0f });
		data.model = m_localTransformMatrix;
	}

	void init(void) override {
		set_position({2.0f, 2.0f, 2.0f});
		set_perspective();
		add_to_update_queue(FUNC_PTR(update();));
	}
};
 */

class CameraScript : public lyra::Script<lyra::Camera> {
	void init(void) override {
		node->set_position({2.0f, 2.0f, 2.0f});
		node->set_perspective();
	}

	void update(void) override {
		node->rotate({ 0.0f, 0.0f, 60 * 90.0f });
		node->look_at({ 0.0f, 0.0f, 0.0f });
	}
};

class Application : public lyra::Application {
	void init() override;
};

int main() { // Cathedral of Assets, Assets Manor or Mansion of Assets, whatever you want to call this SMT reference
	lyra::init();

	// init application
	Application app;

	lyra::Spatial scene("Root"); // I'm sorry godot devs

	lyra::Camera camera("Camera", &scene, new CameraScript());

	lyra::Texture roomTexture("data/img/viking_room.png");

	lyra::Spatial room("Room", &scene);
	lyra::Mesh roomMesh("data/model/viking_room.obj", 0, "RoomMesh", &room);
	lyra::MeshRenderer roomRenderer(&roomMesh, "MeshRenderer", &room);

	// material
	lyra::Material material(&camera, { &roomRenderer }, lyra::Color(), &roomTexture);

	lyra::gui::GUIRenderer guiRenderer;
	guiRenderer.add_draw_call(FUNC_PTR(ImGui::ShowDemoWindow();));

	app.draw();

	lyra::Logger::log_info("Gather Assets and come again.");

	return 0;
}

void Application::init() {
	lyra::Logger::log_info("Welcome to the Lyra Engine Content Manager, where Assets gather... ");
}
