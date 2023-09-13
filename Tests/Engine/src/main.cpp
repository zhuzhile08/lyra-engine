#define SDL_MAIN_HANDLED

/*

#include <Common/FunctionPointer.h>

#include <Common/Common.h>
#include <Math/LyraMath.h>
#include <Application/Application.h>
#include <Graphics/VulkanImpl/DescriptorSystem.h>
#include <Graphics/VulkanImpl/Window.h>
#include <Graphics/GraphicsPipelineSystem.h>
#include <Graphics/VulkanImpl/PipelineBase.h>

#include <Resource/Shader.h>
#include <Resource/Material.h>
#include <Graphics/Texture.h>
#include <Resource/Mesh.h>
#include <Resource/ResourceSystem.h>

#include <EntitySystem/Script.h>
#include <EntitySystem/Entity.h>
#include <EntitySystem/Transform.h>
#include <EntitySystem/MeshRenderer.h>
#include <EntitySystem/Camera.h>
// #include <EntitySystem/Cubemap.h>

#include <Input/InputSystem.h>

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

	lyra::Mesh* femcMesh = lyra::ResourceSystem::mesh("data/mesh/femc.obj");
	lyra::Mesh* roomMesh = lyra::ResourceSystem::mesh("data/mesh/viking_room.obj");

	lyra::Material* femcMaterial = lyra::ResourceSystem::material("data/mesh/femc.mtl");
	lyra::Material roomMaterial(lyra::Color(0, 0, 0, 0), "data/img/viking_room.png");

	lyra::Entity scene("Root");

	lyra::Entity camera("Camera", &scene, new CameraScript);
	camera.add_component<lyra::Camera>();

	lyra::Entity femc("FEMC", &scene);
	femc.add_component<lyra::MeshRenderer>(femcMesh, femcMaterial);

	lyra::Entity room("Room", &scene);
	room.add_component<lyra::MeshRenderer>(roomMesh, &roomMaterial);

	app.draw();

	lyra::log().info("Gather Assets and come again.");

	// lyra::gui::ImGuiRenderer guiRenderer;
	// guiRenderer.add_draw_call(FUNC_PTR(ImGui::ShowDemoWindow();));

	return 0;
}

void Application::init() {
	lyra::log().info("Welcome to the Lyra Engine Content Manager, where Assets gather... ");
}

*/

#include <Lyra/Lyra.h>
#include <Common/Common.h>
#include <Common/FileSystem.h>

#include <Graphics/VulkanRenderSystem.h>
#include <Graphics/SDLWindow.h>
#include <Input/InputSystem.h>

int main(int argc, char* argv[]) {
	lyra::init();
	lyra::initLoggingSystem();
	lyra::initFileSystem(argv);

	lyra::Window window;

	lyra::initInputSystem(window);
	lyra::initRenderSystem({{0, 7, 0}, &window});

	lyra::vulkan::Framebuffers framebuffers;

	lyra::CharVectorStream vertexShaderFile("data/shader/vert.spv", lyra::OpenMode::readBin);
	lyra::vulkan::Shader vertexShader(lyra::vulkan::Shader::Type::vertex, vertexShaderFile.data());

	lyra::CharVectorStream fragmentShaderFile("data/shader/frag.spv", lyra::OpenMode::readBin);
	lyra::vulkan::Shader fragmentShader(lyra::vulkan::Shader::Type::fragment, fragmentShaderFile.data());

	lyra::vulkan::GraphicsProgram graphicsProgram(vertexShader, fragmentShader);

	lyra::vulkan::DescriptorPools descriptorPools({{lyra::vulkan::DescriptorWriter::Type::imageSampler, 2}, {lyra::vulkan::DescriptorWriter::Type::uniformBuffer}});

	lyra::vulkan::GraphicsPipeline::Builder pipelineBuilder(framebuffers);
	pipelineBuilder.setScissor({{lyra::config::windowWidth, lyra::config::windowHeight}});
	pipelineBuilder.setViewport({{lyra::config::windowWidth, lyra::config::windowHeight}});
	lyra::vulkan::GraphicsPipeline graphicsPipeline(graphicsProgram, pipelineBuilder);

	while (window.running()) {
		lyra::input::update();
		if (!lyra::beginFrame()) continue;
		
		framebuffers.begin();

		graphicsPipeline.bind();

		framebuffers.end();

		lyra::endFrame();
	}

	lyra::quit();

	return 0;
}
