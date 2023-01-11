#define SDL_MAIN_HANDLED
#define LYRA_LOG_FILE

#include <core/function_pointer.h>

#include <lyra.h>
#include <math/math.h>
#include <core/application.h>
#include <rendering/material.h>
#include <rendering/vulkan/descriptor.h>
#include <rendering/vulkan/vulkan_window.h>
#include <rendering/graphics_pipeline.h>
#include <rendering/vulkan/vulkan_shader.h>
#include <rendering/vulkan/vulkan_pipeline.h>
#include <rendering/gui_renderer.h>
#include <nodes/mesh/mesh.h>
#include <nodes/mesh/mesh_renderer.h>
#include <nodes/graphics/camera.h>
#include <nodes/graphics/cubemap.h>
#include <nodes/spatial.h>
#include <rendering/texture.h>
#include <input/input.h>
#include <imgui.h>

#include <array>

struct CameraScript : public lyra::Script {
	void update(void) override {
		node->transform.rotate_z(10.0f);
		node->transform.look_at_from_translation({2.0f, 2.0f, 2.0f}, {0.0f, 0.0f, 0.0f});
		
	}
};

class Application : public lyra::Application {
	void init() override;
};

float foo(float a, int b) {
	return a + b;
}

int main() { // Cathedral of Assets, Assets Manor or Mansion of Assets, whatever you want to call this SMT reference
	// init application
	Application app;

	lyra::Spatial scene(nullptr, "Root"); // I'm sorry godot devs

	// lyra::gui::GUIRenderer guiRenderer;
	// guiRenderer.add_draw_call(FUNC_PTR(ImGui::ShowDemoWindow();));

	lyra::Camera camera(new CameraScript, nullptr, true, "Camera", &scene);

	lyra::Assets assets;

	lyra::Spatial room(nullptr, "Room", &scene);
	lyra::Texture* roomTexture = assets["data/img/viking_room.png"];
	lyra::Mesh roomMesh("data/model/viking_room.obj", nullptr, "RoomMesh", &room);
	lyra::MeshRenderer roomRenderer(&roomMesh, nullptr, "MeshRenderer", &room);

	// material
	lyra::Material material(&camera, { &roomRenderer }, lyra::Color(0, 0, 0, 0), roomTexture);

	app.draw();

	lyra::log().info("Gather Assets and come again.");

	return 0;
}

void Application::init() {
	lyra::log().info("Welcome to the Lyra Engine Content Manager, where Assets gather... ");
}
