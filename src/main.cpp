#define SDL_MAIN_HANDLED

#include <lyra.h>
#include <core/logger.h>
#include <core/rendering/vulkan/descriptor.h>
#include <components/graphics/material.h>
#include <core/queue_types.h>
#include <graphics/renderer.h>
#include <graphics/asset_manager.h>
#include <components/mesh/mesh.h>
#include <graphics/texture.h>
#include <math/math.h>
#include <glm.hpp>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

int main() {
	// init application
	lyra::Application::init();

	// renderer
	lyra::Renderer renderer;

	// asset manager
	lyra::AssetManager manager;

	// texture
	lyra::Texture texture;
	texture.create({ "data/img/viking_room.png" });

	// graphics pipeline
	lyra::GraphicsPipeline graphicsPipeline;
	lyra::VulkanDescriptorSetLayout::Builder layoutBuilder;
	layoutBuilder.add_binding({
		{ 0, lyra::VulkanDescriptor::Type::TYPE_UNIFORM_BUFFER, lyra::VulkanShader::Type::TYPE_VERTEX, 1 },
		{ 1, lyra::VulkanDescriptor::Type::TYPE_IMAGE_SAMPLER, lyra::VulkanShader::Type::TYPE_FRAGMENT, 1 }
		});

	lyra::VulkanDescriptorPool::Builder poolBuilder;
	poolBuilder.set_max_sets(4);
	poolBuilder.add_pool_sizes({
		{ lyra::VulkanDescriptor::Type::TYPE_UNIFORM_BUFFER, lyra::Settings::Rendering::maxFramesInFlight },
		{ lyra::VulkanDescriptor::Type::TYPE_IMAGE_SAMPLER, lyra::Settings::Rendering::maxFramesInFlight }
	});

	graphicsPipeline.create({
		&renderer,
		{ { lyra::VulkanShader::Type::TYPE_VERTEX, "data/shader/vert.spv", "main" }, 
		{ lyra::VulkanShader::Type::TYPE_FRAGMENT, "data/shader/frag.spv", "main" } },
		layoutBuilder,
		poolBuilder,
		lyra::Application::context()->swapchain()->extent(),
		lyra::Application::context()->swapchain()->extent()
	});

	// add these to the manager
	manager.add_pipelines({ &graphicsPipeline });
	manager.add_textures({ &texture });

	// camera
	lyra::Camera camera;
	camera.set_position({ 0.f,-6.f,-10.f });
	camera.set_perspective();

	lyra::VulkanDescriptor::Writer writer;
	writer.add_buffer_write(&camera.buffers().at(0).get_descriptor_buffer_info(), 0, lyra::VulkanDescriptor::Type::TYPE_UNIFORM_BUFFER); 
	writer.add_buffer_write(&camera.buffers().at(1).get_descriptor_buffer_info(), 0, lyra::VulkanDescriptor::Type::TYPE_UNIFORM_BUFFER);
	writer.add_image_write(&texture.get_descriptor_image_info());

	// manager
	lyra::Material material;
	material.create(&manager, 0, 0, writer);

	lyra::Mesh room;
	room.create("data/model/viking_room.obj");

	material.bind(&renderer);
	room.bind(&renderer);

	renderer.draw();

	lyra::Application::draw();

	_CrtDumpMemoryLeaks();

	return 0;
}
