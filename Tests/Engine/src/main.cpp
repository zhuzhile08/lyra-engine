#define SDL_MAIN_HANDLED
#define LYRA_LOG_FILE

#include <Common/FunctionPointer.h>

#include <Lyra/Lyra.h>
#include <Math/LyraMath.h>
#include <Application/Application.h>
#include <Graphics/VulkanImpl/DescriptorSystem.h>
#include <Graphics/VulkanImpl/Window.h>
#include <Graphics/GraphicsPipeline.h>
#include <Graphics/VulkanImpl/Shader.h>
#include <Graphics/VulkanImpl/PipelineBase.h>

#include <Resource/Material.h>
#include <Resource/Texture.h>
#include <Resource/Mesh.h>

#include <EntitySystem/Script.h>
#include <EntitySystem/Entity.h>
#include <EntitySystem/Transform.h>
#include <EntitySystem/MeshRenderer.h>
#include <EntitySystem/Camera.h>
// #include <EntitySystem/Cubemap.h>

#include <Input/Input.h>

#include <imgui.h>


struct CameraScript : public lyra::Script {
	void init(void) override {
		transform = node->component<lyra::Transform>();
	}

	void update(void) override { 
		// translation
		if (lyra::input::InputManager::is_key_held(lyra::input::Keyboard::KEYBOARD_W)) {
			transform->translation = 0.1f * transform->front();
		} if (lyra::input::InputManager::is_key_held(lyra::input::Keyboard::KEYBOARD_S)) {
			transform->translation = 0.1f * transform->back();
		} if (lyra::input::InputManager::is_key_held(lyra::input::Keyboard::KEYBOARD_A)) {
			transform->translation = 0.1f * transform->left();
		} if (lyra::input::InputManager::is_key_held(lyra::input::Keyboard::KEYBOARD_D)) {
			transform->translation = 0.1f * transform->right();
		}
		// rotation
		if (lyra::input::InputManager::is_key_held(lyra::input::Keyboard::KEYBOARD_RIGHT)) {
			transform->rotate(glm::vec3(0, 1, 0), 0.05f);
		} if (lyra::input::InputManager::is_key_held(lyra::input::Keyboard::KEYBOARD_LEFT)) {
			transform->rotate(glm::vec3(0, -1, 0), 0.05f);
		} if (lyra::input::InputManager::is_key_held(lyra::input::Keyboard::KEYBOARD_UP)) {
			transform->rotate(glm::vec3(0, 0, 1), 0.05f);
		} if (lyra::input::InputManager::is_key_held(lyra::input::Keyboard::KEYBOARD_DOWN)) {
			transform->rotate(glm::vec3(0, 0, -1), 0.05f);
		}
	}

	lyra::Transform* transform;
};

class Application : public lyra::Application {
	void init() override;
};

int main() {
	// init application
	Application app;

	lyra::Texture* roomTexture = lyra::ResourceManager::texture("data/img/viking_room.png");
	lyra::Texture* femcTexture = lyra::ResourceManager::texture("data/img/p3_femc_sees_tex.png");

	lyra::Mesh* femcMesh = lyra::ResourceManager::mesh("data/model/femc.obj");
	lyra::Mesh* roomMesh = lyra::ResourceManager::mesh("data/model/viking_room.obj");

	lyra::Material femcMaterial(lyra::Color(0, 0, 0, 0), femcTexture);
	lyra::Material roomMaterial(lyra::Color(0, 0, 0, 0), roomTexture);

	lyra::Entity scene("Root");

	lyra::Entity camera("Camera", &scene, new CameraScript);
	camera.add_component<lyra::Camera>();

	lyra::Entity femc("FEMC", &scene);
	femc.add_component<lyra::MeshRenderer>(femcMesh, &femcMaterial);

	lyra::Entity room("Room", &scene);
	room.add_component<lyra::MeshRenderer>(roomMesh, &roomMaterial);

	app.draw();

	lyra::log().info("Gather Assets and come again.");

	// lyra::gui::GUIRenderer guiRenderer;
	// guiRenderer.add_draw_call(FUNC_PTR(ImGui::ShowDemoWindow();));

	return 0;
}

void Application::init() {
	lyra::log().info("Welcome to the Lyra Engine Content Manager, where Assets gather... ");
}
