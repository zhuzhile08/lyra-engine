#include <Lyra/Lyra.h>
#include <Common/Common.h>
#include <Common/FileSystem.h>
#include <Common/Benchmark.h>

#include <Graphics/VulkanRenderSystem.h>
#include <Graphics/Renderer.h>
#include <Graphics/SDLWindow.h>
#include <Graphics/Texture.h>
#include <Graphics/Mesh.h>

#include <EntitySystem/Camera.h>

#include <Resource/ResourceSystem.h>

#include <Input/InputSystem.h>

int main(int argc, char* argv[]) {
	lyra::init();
	lyra::initLoggingSystem();
	lyra::initFileSystem(argv);
	lyra::initResourceSystem();

	lyra::Window window;

	lyra::initInputSystem(window);
	lyra::initRenderSystem({{0, 7, 0}, &window});

	auto& vertexShader = lyra::resource::shader("shader/vert.spv");
	auto& fragmentShader = lyra::resource::shader("shader/frag.spv");

	auto& texture = lyra::resource::texture("img/viking_room.png");
	auto& room = lyra::resource::mesh("mesh/viking_room.obj", 0);

	lyra::vulkan::Framebuffers framebuffers;
	lyra::vulkan::GraphicsProgram graphicsProgram(vertexShader, fragmentShader);

	lyra::renderSystem::Renderer renderer(framebuffers, graphicsProgram);

	lyra::Camera camera;

	renderer.bindCamera(camera);

	while (window.running()) {
		lyra::input::update();
		if (!lyra::renderSystem::beginFrame()) continue;

		lyra::renderSystem::draw();

		lyra::renderSystem::endFrame();
	}

	lyra::quit();

	return 0;
}
