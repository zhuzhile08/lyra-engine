#include <core/rendering/gui_context.h>

namespace lyra {

namespace gui {

GUIContext::GUIContext(Renderer* const renderer) {
	Logger::log_info("Creating context for the GUI... ");

	this->renderer = renderer;

	// information about the descriptor pool
	VulkanDescriptorPool::Builder builder;
	builder.add_pool_sizes({
		{ VulkanDescriptor::Type::TYPE_SAMPLER, 1000 },
		{ VulkanDescriptor::Type::TYPE_IMAGE_SAMPLER, 1000 },
		{ VulkanDescriptor::Type::TYPE_SAMPLED_IMAGE, 1000 },
		{ VulkanDescriptor::Type::TYPE_STORAGE_IMAGE, 1000 },
		{ VulkanDescriptor::Type::TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VulkanDescriptor::Type::TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VulkanDescriptor::Type::TYPE_UNIFORM_BUFFER, 1000 },
		{ VulkanDescriptor::Type::TYPE_STORAGE_BUFFER, 1000 },
		{ VulkanDescriptor::Type::TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VulkanDescriptor::Type::TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VulkanDescriptor::Type::TYPE_INPUT_ATTACHMENT, 1000 }
		});
	builder.set_max_sets(1000); // I think this may be a bit too much, but welp, imgui tells me this is a good idea
	builder.set_pool_flags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);
	// create the descriptor pool
	_descriptorPool.create(builder);

	// initialize ImGui
	ImGui::CreateContext();
	// initialize ImGui for SDL
	ImGui_ImplSDL2_InitForVulkan(Application::window()->get());
	// initialization information
	ImGui_ImplVulkan_InitInfo initInfo{
		Application::context()->instance()->instance(),
		Application::context()->device()->physicalDevice(),
		Application::context()->device()->device(),
		Application::context()->device()->graphicsQueue().familyIndex,
		Application::context()->device()->graphicsQueue().queue,
		VK_NULL_HANDLE,
		_descriptorPool.get(),
		0,
		3,
		3,
		Application::context()->swapchain()->colorResources()->maxSamples()
	};
	// initialize ImGui for Vulkan
	ImGui_ImplVulkan_Init(&initInfo, renderer->framebuffers()->renderPass());

	// create a temporary command buffer for creating the font textures
	VulkanCommandBuffer cmdBuff;
	cmdBuff.create(Application::context()->device(), Application::context()->commandPool());
	// start recording the command buffer
	cmdBuff.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	// create the textures
	ImGui_ImplVulkan_CreateFontsTexture(cmdBuff.get());
	// end recording the command buffer
	cmdBuff.end();
	// submit the commands
	cmdBuff.submit_queue(Application::context()->device()->graphicsQueue().queue);
	cmdBuff.wait_queue(Application::context()->device()->graphicsQueue().queue);

	// destroy font data after creating
	ImGui_ImplVulkan_DestroyFontUploadObjects();

	renderer->add_to_draw_queue(FUNC_PTR( ImGui::Render(); ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), Application::context()->commandBuffers().at(Application::context()->currentFrame()).get()); ));
	const_cast<Window*>(Application::window())->check_events(FUNC_PTR( ImGui_ImplSDL2_ProcessEvent(&Application::window()->event()); ));

	Logger::log_info("Successfully created a GUI context at: ", get_address(this));
}

void GUIContext::bind() const {
	// render a new frame
	renderer->add_to_update_queue([&] {
		// get inputs

		// begin drawing
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL2_NewFrame(Application::window()->get());
		ImGui::NewFrame();

		_drawQueue.flush();
		});
}

} // namespace gui

} // namespace lyra
