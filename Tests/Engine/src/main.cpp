#include <Lyra/Lyra.h>
#include <Common/Common.h>
#include <Common/FileSystem.h>
#include <Common/Benchmark.h>

#include <Graphics/VulkanRenderSystem.h>
#include <Graphics/Renderer.h>
#include <Graphics/SDLWindow.h>
#include <Graphics/Material.h>
#include <Graphics/Texture.h>
#include <Graphics/Mesh.h>

#include <EntitySystem/Entity.h>
#include <EntitySystem/Transform.h>
#include <EntitySystem/Camera.h>
#include <EntitySystem/MeshRenderer.h>

#include <Resource/ResourceSystem.h>

#include <Input/InputSystem.h>

int main(int argc, char* argv[]) {
	lyra::init();
	lyra::initLoggingSystem();
	lyra::initFileSystem(argv);
	lyra::initResourceSystem();

	lyra::Window window;

	lyra::initInputSystem(window);
	lyra::initRenderSystem({0, 7, 0}, window);

	lyra::Entity sceneRoot;

	lyra::Entity cameraArm (sceneRoot, "CameraArm");

	lyra::Entity camera (cameraArm, "Camera");
	camera.addComponent<lyra::Camera>();
    
    camera.component<lyra::Transform>()->translation = { 10.0f, 10.0f, 10.0f };
    camera.component<lyra::Transform>()->lookAt({ 0.0f, 0.0f, 3.0f });
    
    lyra::Material material(
        lyra::Color(),
        { &lyra::resource::texture("img/viking_room.png") } // very slow, 6 seconds
    );

	lyra::Mesh quad(
		{
    		{ { -0.5f, -0.5f, 0.0f }, glm::vec3(1), { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
    		{ { 0.5f, -0.5f, 0.0f }, glm::vec3(1), { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } },
    		{ { 0.5f, 0.5f, 0.0f }, glm::vec3(1), { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } },
   			{ { -0.5f, 0.5f, 0.0f }, glm::vec3(1), { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 0.0f } }
        },
		{
			0, 1, 2, 2, 3, 0
		}
	);

	lyra::Entity meshRenderer (sceneRoot, "MeshRenderer");
	// meshRenderer.addComponent<lyra::MeshRenderer>(quad, material);
    meshRenderer.addComponent<lyra::MeshRenderer>(lyra::resource::mesh("mesh/viking_room.obj", 0), material);
    
    // meshRenderer.component<lyra::Transform>()->scale.z *= -1;

	lyra::renderer::setScene(sceneRoot);

	while (window.running()) {
		lyra::input::update();
		if (!lyra::renderer::beginFrame()) continue;
        
		lyra::renderer::draw();
        
        cameraArm.component<lyra::Transform>()->rotate({ 0.0f, 0.0f, 1.0f }, lyra::renderer::deltaTime() * 1.0f);

		lyra::renderer::endFrame();
	}

	lyra::quit();

	return 0;
}
