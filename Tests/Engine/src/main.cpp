#define SDL_MAIN_HANDLED

#include <Lyra/Lyra.h>
#include <Common/Common.h>
#include <Common/FileSystem.h>

#include <Graphics/VulkanRenderSystem.h>
#include <Graphics/SDLWindow.h>
#include <Graphics/Texture.h>

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

	lyra::Texture texture(lyra::resource::texture("img/viking_room.png"));

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
