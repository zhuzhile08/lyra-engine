#include <core/rendering/gui_renderer.h>

#include <core/context.h>

#include <core/logger.h>

#include <core/queue_types.h>

#include <core/rendering/vulkan/devices.h>
#include <core/rendering/vulkan/vulkan_window.h>
#include <core/rendering/vulkan/descriptor.h>
#include <core/rendering/vulkan/command_buffer.h>
#include <core/rendering/renderer.h>

namespace lyra {

namespace gui {

GUIRenderer::GUIRenderer() {
	Logger::log_info("Creating context for the GUI... ");

	_renderer = std::make_unique<Renderer>();
	_drawQueue = std::make_unique<CallQueue>();

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
	_descriptorPool = std::make_unique<VulkanDescriptorPool>(builder);

	// initialize ImGui
	ImGui::CreateContext();
	// initialize ImGui for SDL
	ImGui_ImplSDL2_InitForVulkan(Context::get()->window()->get());
	// initialization information
	ImGui_ImplVulkan_InitInfo initInfo{
		Context::get()->renderSystem()->device()->instance(),
		Context::get()->renderSystem()->device()->physicalDevice(),
		Context::get()->renderSystem()->device()->device(),
		Context::get()->renderSystem()->device()->graphicsQueue().familyIndex,
		Context::get()->renderSystem()->device()->graphicsQueue().queue,
		VK_NULL_HANDLE,
		_descriptorPool->get(),
		0,
		3,
		3,
		Context::get()->renderSystem()->vulkanWindow()->maxMultisamples()
	};
	// initialize ImGui for Vulkan
	ImGui_ImplVulkan_Init(&initInfo, _renderer->renderPass());

	// get a command buffer for creating the font textures
	CommandBuffer cmdBuff = Context::get()->renderSystem()->commandBuffers()->get_unused();
	// start recording the command buffer
	Context::get()->renderSystem()->commandBuffers()->begin(cmdBuff, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	// create the textures
	ImGui_ImplVulkan_CreateFontsTexture(Context::get()->renderSystem()->commandBuffers()->commandBuffer(cmdBuff)->commandBuffer);
	// end recording the command buffer
	Context::get()->renderSystem()->commandBuffers()->end(cmdBuff);
	// submit the commands
	Context::get()->renderSystem()->commandBuffers()->submit_queue(cmdBuff, Context::get()->renderSystem()->device()->graphicsQueue().queue);
	Context::get()->renderSystem()->commandBuffers()->wait_queue(Context::get()->renderSystem()->device()->graphicsQueue().queue);
	// reset the command buffer
	Context::get()->renderSystem()->commandBuffers()->reset(cmdBuff);

	// destroy font data after creating
	ImGui_ImplVulkan_DestroyFontUploadObjects();
	
	const_cast<Window*>(Context::get()->window())->check_events(FUNC_PTR( ImGui_ImplSDL2_ProcessEvent(&Context::get()->window()->event()); ));
	// bind to the renderer
	bind();

	Logger::log_info("Successfully created a GUI context at: ", get_address(this));
}

GUIRenderer::~GUIRenderer() {
	ImGui_ImplVulkan_Shutdown();

	lyra::Logger::log_info("Successfully destroyed GUI context!");
}

void GUIRenderer::add_draw_call(std::function<void()>&& func) {
	_drawQueue->add(std::move(func));
}

void GUIRenderer::bind() {
	// render a new frame
	_renderer->add_to_draw_queue(FUNC_PTR(
		// begin drawing
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL2_NewFrame(Context::get()->window()->get());
		ImGui::NewFrame();

		_drawQueue->flush();

		ImGui::Render(); 
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), Context::get()->renderSystem()->activeCommandBuffer());
		));
}

} // namespace gui

} // namespace lyra
