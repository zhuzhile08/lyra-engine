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

	lyra::Entity camera (sceneRoot, "Camera");
	camera.addComponent<lyra::Camera>();

	lyra::Material roomMat(
		lyra::Color(),
		{ &lyra::resource::texture("img/viking_room.png") }
	);

	// lyra::Mesh quad(
	//	{
    //		{ { -0.5f, -0.5f, 0.0f }, glm::vec3(1), { 1.0f, 0.0f, 0.0f }, glm::vec3(1) },
    //		{ { 0.5f, -0.5f, 0.0f }, glm::vec3(1), { 0.0f, 1.0f, 0.0f }, glm::vec3(1) },
    //		{ { 0.5f, 0.5f, 0.0f }, glm::vec3(1), { 0.0f, 0.0f, 1.0f }, glm::vec3(1) },
   	//		{ { -0.5f, 0.5f, 0.0f }, glm::vec3(1), { 1.0f, 1.0f, 1.0f }, glm::vec3(1) }
	//	},
	//	{
	//		0, 1, 2, 2, 3, 0
	//	}
	// );

	lyra::Entity room (sceneRoot, "Room");
	// room.addComponent<lyra::MeshRenderer>(quad, roomMat);
	room.addComponent<lyra::MeshRenderer>(lyra::resource::mesh("mesh/viking_room.obj", 0), roomMat);

	lyra::renderer::setScene(sceneRoot);

	while (window.running()) {
		lyra::input::update();
		if (!lyra::renderer::beginFrame()) continue;

		lyra::renderer::draw();

		camera.component<lyra::Transform>()->normalizeAndRotate({ 0.0f, 0.0f, 1.0f }, lyra::renderer::deltaTime() * 90.0f);
		camera.component<lyra::Transform>()->lookAt({ 0.0f, 0.0f, 0.0f });

		lyra::renderer::endFrame();
	}

	lyra::quit();

	return 0;
}
