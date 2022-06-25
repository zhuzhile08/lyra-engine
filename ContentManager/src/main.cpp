#define SDL_MAIN_HANDLED

#include <lyra.h>
#include <graphics/renderer.h>
#include <core/rendering/gui_context.h>
#include <components/graphics/material.h>
#include <graphics/material_manager.h>
#include <components/mesh/mesh.h>
#include <components/graphics/camera.h>
#include <components/gameObj.h>
#include <graphics/texture.h>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


class Camera : public lyra::GameObject, public lyra::Camera {
public:
	Camera() { init(); }

	void init(void) override {
		set_position({2.0f, 2.0f, 2.0f});
	}

	void update(void) override {	
		CameraData data;
		look_at({ 0.0f, 0.0f, 0.0f });
		// _rotation.y += lyra::FPS() * glm::radians(90.0f);
		rotate({ 0.0f, 0.0f, lyra::FPS() * 90.0f });
		lyra::Logger::log_info("rotation: ", " x: ", _rotation.x, " y: ", _rotation.y, " z: ", _rotation.z);
		data.model = _localTransformMatrix;
		draw(data);
	}
};


int main() { // Cathedral of Assets, Assets Manor or Mansion of Assets, whatever you want to call this SMT reference
	lyra::Logger::log_info("Welcome to the Lyra Engine Content Manager, where Assets gather... ");

	// init application
	lyra::Application::init();

	// GUI
	lyra::gui::GUIContext gui;

	// renderer
	lyra::Renderer renderer;

	// camera
	Camera camera;
	renderer.add_to_update_queue([&]() { camera.update(); });

	// asset manager
	lyra::MaterialManager manager;

	// texture
	lyra::Texture texture;
	texture.create({ "data/img/viking_room.png" });

	// graphics pipeline
	lyra::GraphicsPipeline graphicsPipeline;
	lyra::VulkanDescriptorSetLayout::Builder layoutBuilder;
	lyra::VulkanDescriptorPool::Builder poolBuilder;
	layoutBuilder.add_binding({
		{ 0, lyra::VulkanDescriptor::Type::TYPE_UNIFORM_BUFFER, lyra::VulkanShader::Type::TYPE_VERTEX, 1 },
		{ 1, lyra::VulkanDescriptor::Type::TYPE_IMAGE_SAMPLER, lyra::VulkanShader::Type::TYPE_FRAGMENT, 1 }
		});
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
	
	// manager
	lyra::Material material;
	lyra::VulkanDescriptor::Writer writer;
	writer.add_writes({
		{ nullptr, &camera.buffers().at(0).get_descriptor_buffer_info(), 0, lyra::VulkanDescriptor::Type::TYPE_UNIFORM_BUFFER },
		{ nullptr, &camera.buffers().at(1).get_descriptor_buffer_info(), 0, lyra::VulkanDescriptor::Type::TYPE_UNIFORM_BUFFER },
		{ &texture.get_descriptor_image_info(), nullptr, 1, lyra::VulkanDescriptor::Type::TYPE_IMAGE_SAMPLER }
	});
	material.create(&manager, 0, 0, writer);
	material.bind(&renderer);

	lyra::Mesh room;
	room.create("data/model/viking_room.obj");
	room.bind(&renderer);

	renderer.bind();

	gui.draw();

	lyra::Application::draw();

	lyra::Logger::log_info("Gather Assets and come again.");

	_CrtDumpMemoryLeaks();

	return 0;
}
