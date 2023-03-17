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
// #include <nodes/graphics/cubemap.h>
#include <nodes/spatial.h>
#include <rendering/texture.h>
#include <input/input.h>
#include <imgui.h>

#include <array>

struct CameraMovementScript : public lyra::Script {
	void init(void) override {
		// node->transform.translate({3.0f, 5.0f, 3.0f});
	}

	void update(void) override { 
		if (lyra::input::InputManager::is_key_held(lyra::input::Keyboard::KEYBOARD_W)) {
			node->transform.translate(0.1f * node->transform.forward());
		} if (lyra::input::InputManager::is_key_held(lyra::input::Keyboard::KEYBOARD_S)) {
			node->transform.translate(0.1f * node->transform.back());
		} if (lyra::input::InputManager::is_key_held(lyra::input::Keyboard::KEYBOARD_A)) {
			node->transform.translate(0.1f * node->transform.left());
		} if (lyra::input::InputManager::is_key_held(lyra::input::Keyboard::KEYBOARD_D)) {
			node->transform.translate(0.1f * node->transform.right());
		}
	}
};

struct CameraRotationScript : public lyra::Script {
	void init(void) override {
		// node->transform.look_at({0.0f, 2.2f, 0.0f});
	}

	void update(void) override { 
		if (lyra::input::InputManager::is_key_held(lyra::input::Keyboard::KEYBOARD_RIGHT)) {
			node->transform.rotate(0.05f * glm::vec3(0, 1, 0));
		} if (lyra::input::InputManager::is_key_held(lyra::input::Keyboard::KEYBOARD_LEFT)) {
			node->transform.rotate(0.05f * glm::vec3(0, -1, 0));
		} if (lyra::input::InputManager::is_key_held(lyra::input::Keyboard::KEYBOARD_UP)) {
			node->transform.rotate(0.05f * glm::vec3(0, 0, 1));
		} if (lyra::input::InputManager::is_key_held(lyra::input::Keyboard::KEYBOARD_DOWN)) {
			node->transform.rotate(0.05f * glm::vec3(0, 0, -1));
		}
	}
};

class Application : public lyra::Application {
	void init() override;
};

int main() { // Cathedral of Assets, Assets Manor or Mansion of Assets, whatever you want to call this SMT reference
	// init application
	Application app;

	lyra::Spatial scene(nullptr, "Root"); // I'm sorry godot devs

	// lyra::gui::GUIRenderer guiRenderer;
	// guiRenderer.add_draw_call(FUNC_PTR(ImGui::ShowDemoWindow();));

	lyra::Spatial cameraArm(new CameraMovementScript, "Camera Arm", &scene);
	lyra::Camera camera(new CameraRotationScript, nullptr, true, "Camera", &cameraArm);

	lyra::Assets assets;

	lyra::Spatial femc(nullptr, "FEMC", &scene);
	lyra::Texture* femcTexture = assets["data/img/p3_femc_sees_tex.png"];
	lyra::Mesh femcMesh("data/model/femc.obj", nullptr, "FEMCMesh", &femc);
	lyra::MeshRenderer femcRenderer(&femcMesh, nullptr, "FEMCRenderer", &femc);
	lyra::Material femcMaterial(&camera, { &femcRenderer }, lyra::Color(0, 0, 0, 0), femcTexture);

	lyra::Spatial room(nullptr, "Room", &scene);
	lyra::Texture* roomTexture = assets["data/img/viking_room.png"];
	lyra::Mesh roomMesh("data/model/viking_room.obj", nullptr, "RoomMesh", &room);
	lyra::MeshRenderer roomRenderer(&roomMesh, nullptr, "MeshRenderer", &room);
	lyra::Material roomMaterial(&camera, { &roomRenderer }, lyra::Color(0, 0, 0, 0), roomTexture);

	app.draw();

	lyra::log().info("Gather Assets and come again.");

	return 0;
}

void Application::init() {
	lyra::log().info("Welcome to the Lyra Engine Content Manager, where Assets gather... ");
}
