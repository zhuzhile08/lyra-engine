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

	/**

	auto& vertexShader = lyra::resource::shader("shader/vert.spv");
	auto& fragmentShader = lyra::resource::shader("shader/frag.spv");

	auto& texture = lyra::resource::texture("img/viking_room.png");
	auto& room = lyra::resource::mesh("mesh/viking_room.obj", 0);

	lyra::vulkan::Framebuffers framebuffers;
	lyra::vulkan::GraphicsProgram graphicsProgram(vertexShader, fragmentShader);

	lyra::renderSystem::Renderer renderer(framebuffers, graphicsProgram);

	lyra::Material roomMat(
		lyra::Color(),
		{ &texture }
	);

	lyra::Camera camera;

	renderer.bindCamera(camera);

	lyra::MeshRenderer roomRenderer(room, roomMat);

	renderer.m_meshes.emplace(&roomMat, std::vector<lyra::MeshRenderer*>{ &roomRenderer });

	*/

	auto p = lyra::renderer::graphicsPipeline(lyra::resource::shader("shader/vert.spv"), lyra::resource::shader("shader/frag.spv"));

	lyra::renderer::setScene(sceneRoot);

	while (window.running()) {
		lyra::input::update();
		if (!lyra::renderer::beginFrame()) continue;

		lyra::renderer::draw();

		lyra::renderer::endFrame();
	}

	lyra::quit();

	return 0;
}
