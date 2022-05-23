#include <core/rendering/gui_context.h>

namespace lyra {

namespace gui {

GUIContext::GUIContext() { }

GUIContext::~GUIContext() {
	ImGui_ImplVulkan_Shutdown();

	lyra::Logger::log_info("Successfully destroyed GUI context!");
}

void GUIContext::destroy() {
	this->~GUIContext();
}

void GUIContext::create(lyra::Context* const context, const lyra::Window* const window) {
	Logger::log_info("Creating context for the GUI... ");

	this->context = context;
	this->window = window;

	_framebuffers.create(context->device(), context->swapchain());

	// information about the descriptor pool
	VulkanDescriptorPool::Builder builder;
	builder.add_pool_sizes({
		{ VulkanDescriptorPool::Type::TYPE_SAMPLER, 1000 },
		{ VulkanDescriptorPool::Type::TYPE_IMAGE_SAMPLER, 1000 },
		{ VulkanDescriptorPool::Type::TYPE_SAMPLED_IMAGE, 1000 },
		{ VulkanDescriptorPool::Type::TYPE_STORAGE_IMAGE, 1000 },
		{ VulkanDescriptorPool::Type::TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VulkanDescriptorPool::Type::TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VulkanDescriptorPool::Type::TYPE_UNIFORM_BUFFER, 1000 },
		{ VulkanDescriptorPool::Type::TYPE_STORAGE_BUFFER, 1000 },
		{ VulkanDescriptorPool::Type::TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VulkanDescriptorPool::Type::TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VulkanDescriptorPool::Type::TYPE_INPUT_ATTACHMENT, 1000 }
	});
	builder.set_max_sets(1000); // I think this may be a bit too much, but welp, imgui tells me this is a good idea
	builder.set_pool_flags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);
	// create the descriptor pool
	_descriptorPool.create(context->device(), builder);

	// initialize ImGui
	ImGui::CreateContext();
	// initialize ImGui for SDL
	ImGui_ImplSDL2_InitForVulkan(window->get());
	// initialization information
	ImGui_ImplVulkan_InitInfo initInfo{
		context->instance()->instance(),
		context->device()->physicalDevice(),
		context->device()->device(),
		context->device()->graphicsQueue().familyIndex,
		context->device()->graphicsQueue().queue,
		VK_NULL_HANDLE,
		_descriptorPool.get(),
		0,
		3,
		3,
		VK_SAMPLE_COUNT_1_BIT
	};
	// initialize ImGui for Vulkan
	ImGui_ImplVulkan_Init(&initInfo, _framebuffers.renderPass());

	// create a temporary command buffer for creating the font textures
	VulkanCommandBuffer cmdBuff;
	cmdBuff.create(context->device(), context->commandPool());
	// start recording the command buffer
	cmdBuff.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	// create the textures
	ImGui_ImplVulkan_CreateFontsTexture(cmdBuff.get());
	// end recording the command buffer
	cmdBuff.end();
	// submit the commands
	cmdBuff.submit_queue(context->device()->graphicsQueue().queue);
	cmdBuff.wait_queue(context->device()->graphicsQueue().queue);

	// destroy font data after creating
	ImGui_ImplVulkan_DestroyFontUploadObjects();

	Logger::log_info("Successfully created a GUI context at: ", get_address(this));
}

void GUIContext::add_draw_call(std::function<void()>&& func) {
	_drawQueue.add(std::move(func));
}

void GUIContext::draw() {
	// render a new frame
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplSDL2_NewFrame(window->get());
	ImGui::NewFrame();
	
	_drawQueue.flush();
}

} // namespace gui

} // namespace lyra
