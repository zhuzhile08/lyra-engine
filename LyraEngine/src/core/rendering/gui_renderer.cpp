#include <core/rendering/gui_renderer.h>

#include <core/application.h>

#include <core/logger.h>

#include <core/queue_types.h>

#include <core/rendering/vulkan/devices.h>
#include <core/rendering/vulkan/vulkan_window.h>
#include <core/rendering/vulkan/descriptor.h>
#include <core/rendering/vulkan/command_buffer.h>
#include <core/rendering/renderer.h>

namespace lyra {

namespace gui {

GUIRenderer::GUIRenderer() : Renderer() {
	Logger::log_info("Creating context for the GUI... ");

	// information about the descriptor pool
	vulkan::DescriptorPool::Builder builder;
	builder.add_pool_sizes({
		{ vulkan::Descriptor::Type::TYPE_SAMPLER, 1000 },
		{ vulkan::Descriptor::Type::TYPE_IMAGE_SAMPLER, 1000 },
		{ vulkan::Descriptor::Type::TYPE_SAMPLED_IMAGE, 1000 },
		{ vulkan::Descriptor::Type::TYPE_STORAGE_IMAGE, 1000 },
		{ vulkan::Descriptor::Type::TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ vulkan::Descriptor::Type::TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ vulkan::Descriptor::Type::TYPE_UNIFORM_BUFFER, 1000 },
		{ vulkan::Descriptor::Type::TYPE_STORAGE_BUFFER, 1000 },
		{ vulkan::Descriptor::Type::TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ vulkan::Descriptor::Type::TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ vulkan::Descriptor::Type::TYPE_INPUT_ATTACHMENT, 1000 }
		});
	builder.set_max_sets(1000); // I think this may be a bit too much, but welp, imgui tells me this is a good idea
	builder.set_pool_flags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);
	// create the descriptor pool
	m_descriptorPool = SmartPointer<vulkan::DescriptorPool>::create(builder);

	// initialize ImGui
	ImGui::CreateContext();
	// initialize ImGui for SDL
	ImGui_ImplSDL2_InitForVulkan(Application::window()->get());
	// initialization information
	ImGui_ImplVulkan_InitInfo initInfo{
		Application::renderSystem()->device()->instance(),
		Application::renderSystem()->device()->physicalDevice(),
		Application::renderSystem()->device()->device(),
		Application::renderSystem()->device()->graphicsQueue().familyIndex,
		Application::renderSystem()->device()->graphicsQueue().queue,
		VK_NULL_HANDLE,
		m_descriptorPool->get(),
		0,
		3,
		3,
		Application::renderSystem()->vulkanWindow()->maxMultisamples()
	};
	// initialize ImGui for Vulkan
	ImGui_ImplVulkan_Init(&initInfo, m_renderPass);

	// get a command buffer for creating the font textures
	vulkan::CommandBuffer cmdBuff(Application::renderSystem()->commandBuffers());
	// start recording the command buffer
	cmdBuff.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	// create the textures
	ImGui_ImplVulkan_CreateFontsTexture(*cmdBuff.m_commandBuffer);
	// end recording the command buffer
	cmdBuff.end();
	// submit the commands
	cmdBuff.submitQueue(Application::renderSystem()->device()->graphicsQueue().queue);
	// reset the command buffer
	cmdBuff.reset();

	// destroy font data after creating
	ImGui_ImplVulkan_DestroyFontUploadObjects();
	
	const_cast<Window*>(Application::window())->check_events(FUNC_PTR( ImGui_ImplSDL2_ProcessEvent(&Application::window()->event()); ));

	Logger::log_info("Successfully created a GUI context at: ", get_address(this));
}

GUIRenderer::~GUIRenderer() {
	ImGui_ImplVulkan_Shutdown();

	lyra::Logger::log_info("Successfully destroyed GUI context!");
}

void GUIRenderer::add_draw_call(std::function<void()>&& func) {
	m_drawQueue.add(std::move(func));
}

void GUIRenderer::record_command_buffers() const {
	begin_renderpass();

	// begin drawing
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplSDL2_NewFrame(Application::window()->get());
	ImGui::NewFrame();

	// flush all draw commands
	m_drawQueue.flush();

	// render
	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *Application::renderSystem()->currentCommandBuffer().m_commandBuffer);

	end_renderpass();
}

} // namespace gui

} // namespace lyra
