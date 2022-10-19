#define SDL_MAIN_HANDLED
#define LYRA_LOG_FILE

#include <Jolt/Jolt.h>

#include <lyra.h>
#include <math/types/vectors.h>
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
#include <nodes/spatial.h>
#include <rendering/texture.h>
#include <input/input.h>
#include <imgui.h>

#include <array>

class CameraScript : public lyra::Script<lyra::Camera> {
	void init(void) override {
		node->set_position({2.0f, 2.0f, 2.0f});
		node->look_at({0.0f, 0.0f, 0.0f});
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

	lyra::gui::GUIRenderer guiRenderer;
	guiRenderer.add_draw_call(FUNC_PTR(ImGui::ShowDemoWindow();));

	lyra::Camera camera(true, "Camera", &scene, new CameraScript());

	lyra::Texture roomTexture("data/img/viking_room.png");

	lyra::Spatial room("Room", &scene);
	lyra::Mesh roomMesh("data/model/viking_room.obj", 0, "RoomMesh", &room);
	lyra::MeshRenderer roomRenderer(&roomMesh, "MeshRenderer", &room);

	// material
	lyra::Material material(&camera, { &roomRenderer }, lyra::Color(0, 0, 0, 0), &roomTexture);

	app.draw();

	lyra::Logger::log_info("Gather Assets and come again.");

	return 0;
}

void Application::init() {
	lyra::Logger::log_info("Welcome to the Lyra Engine Content Manager, where Assets gather... ");
}
