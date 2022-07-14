#include <core/rendering/gui_context.h>

namespace lyra {

namespace gui {

GUIContext::GUIContext() {
	Logger::log_info("Creating context for the GUI... ");

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
	ImGui_ImplVulkan_Init(&initInfo, _renderer.framebuffers()->renderPass());

	// get a command buffer for creating the font textures
	CommandBuffer cmdBuff = Application::context()->commandBuffers()->get_unused();
	// start recording the command buffer
	Application::context()->commandBuffers()->begin(cmdBuff, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	// create the textures
	ImGui_ImplVulkan_CreateFontsTexture(Application::context()->commandBuffers()->commandBuffer(cmdBuff)->commandBuffer);
	// end recording the command buffer
	Application::context()->commandBuffers()->end(cmdBuff);
	// submit the commands
	Application::context()->commandBuffers()->submit_queue(cmdBuff, Application::context()->device()->graphicsQueue().queue);
	Application::context()->commandBuffers()->wait_queue(cmdBuff, Application::context()->device()->graphicsQueue().queue);
	// reset the command buffer
	Application::context()->commandBuffers()->reset(cmdBuff);

	// destroy font data after creating
	ImGui_ImplVulkan_DestroyFontUploadObjects();
	
	const_cast<Window*>(Application::window())->check_events(FUNC_PTR( ImGui_ImplSDL2_ProcessEvent(&Application::window()->event()); ));
	// bind to the renderer
	bind();
	// bind the renderer
	_renderer.bind();

	Logger::log_info("Successfully created a GUI context at: ", get_address(this));
}

void GUIContext::bind() {
	// render a new frame
	_renderer.add_to_draw_queue(FUNC_PTR(
		// begin drawing
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL2_NewFrame(Application::window()->get());
		ImGui::NewFrame();

		_drawQueue.flush();

		ImGui::Render(); 
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), Application::context()->commandBuffers()->commandBuffer(Application::context()->currentCommandBuffer())->commandBuffer);
		));
}

} // namespace gui

} // namespace lyra
