#define SDL_MAIN_HANDLED

#include <lyra.h>
#include <core/rendering/gui_context.h>
#include <graphics/material.h>
#include <graphics/material_manager.h>
#include <components/mesh/mesh.h>
#include <components/graphics/camera.h>
#include <components/gameObj.h>
#include <graphics/texture.h>
#include <menus.h>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


class Camera : public lyra::GameObject, public lyra::Camera {
public:
	Camera() { init(); }

	void update(void) override {
		CameraData data;
		look_at({ 0.0f, 0.0f, 0.0f });
		rotate({ 0.0f, 0.0f, lyra::Application::fps() * 90.0f });
		data.model = _localTransformMatrix;
		draw(data);
	}

	void init(void) override {
		set_position({2.0f, 2.0f, 2.0f});
		_renderer.add_to_update_queue(FUNC_PTR(update();));
	}
};


int main() { // Cathedral of Assets, Assets Manor or Mansion of Assets, whatever you want to call this SMT reference
	lyra::Logger::log_info("Welcome to the Lyra Engine Content Manager, where Assets gather... ");

	// init application
	lyra::Application::init();

	// camera
	Camera camera;

	// asset manager
	lyra::MaterialManager manager;

	/**  
	// texture
	lyra::Texture texture;
	texture.create({ "data/img/viking_room.png" });

	// graphics pipeline
	lyra::GraphicsPipeline graphicsPipeline;
	lyra::GraphicsPipeline::Builder pipelineBuilder;
	pipelineBuilder.set_max_sets(4);
	pipelineBuilder.add_bindings({
		{ 0, lyra::VulkanDescriptor::Type::TYPE_UNIFORM_BUFFER, lyra::VulkanShader::Type::TYPE_VERTEX, 1, lyra::Settings::Rendering::maxFramesInFlight },
		{ 1, lyra::VulkanDescriptor::Type::TYPE_IMAGE_SAMPLER, lyra::VulkanShader::Type::TYPE_FRAGMENT, 1, lyra::Settings::Rendering::maxFramesInFlight }
		});
	graphicsPipeline.create({
		&renderer,
		{ { lyra::VulkanShader::Type::TYPE_VERTEX, "data/shader/vert.spv", "main" },
		{ lyra::VulkanShader::Type::TYPE_FRAGMENT, "data/shader/frag.spv", "main" } },
		pipelineBuilder,
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
	*/

	// GUI
	lyra::gui::GUIContext gui;
	gui.add_draw_call(FUNC_PTR( Menus::show_window_bar(); ImGui::ShowDemoWindow(); ));

	lyra::Application::draw();

	lyra::Logger::log_info("Gather Assets and come again.");

	_CrtDumpMemoryLeaks();

	return 0;
}
