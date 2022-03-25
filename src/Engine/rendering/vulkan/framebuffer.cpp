#include <rendering/vulkan/framebuffer.h>

namespace lyra {

VulkanFramebuffers::VulkanFramebuffers() { }

void VulkanFramebuffers::destroy() noexcept {
    for (auto framebuffer : var.framebuffers) vkDestroyFramebuffer(device->get().device, framebuffer, nullptr);
    vkDestroyRenderPass(device->get().device, var.renderPass, nullptr);

	delete device;
	delete swapchain;

	LOG_INFO("Succesfully destroyed Vulkan frame buffer!")
}

void VulkanFramebuffers::create(VulkanDevice device, VulkanSwapchain swapchain) {
	this->device = new VulkanDevice(device);
	this->swapchain = new VulkanSwapchain(swapchain);

	create_render_pass();
	create_frame_buffers();

	LOG_INFO("Succesfully created Vulkan framebuffers and render pass at ", GET_ADDRESS(this), "!", END_L)
}

void VulkanFramebuffers::create_render_pass() {
	

	// define what to do with an image during rendering
	VkAttachmentDescription					imageAttachmentDescriptions {
		0,
		swapchain->get().format,
		VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	};

	// depth buffers
	VkAttachmentDescription					depthBufferAttachmentDescriptions {
		0,
		VK_FORMAT_D32_SFLOAT,
		VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};

	VkAttachmentReference 					colorAttachmentReferences {
		0, 
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	VkAttachmentReference 					depthBufferAttachmentReferences {
		1, 
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};

	std::vector<VkAttachmentDescription> 	attachments = {imageAttachmentDescriptions, depthBufferAttachmentDescriptions};

	VkSubpassDescription 					subpassDescriptions {
		0,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		0,
		nullptr,
		1,
		&colorAttachmentReferences,
		nullptr,
		&depthBufferAttachmentReferences,
		0,
		nullptr
	};

	VkSubpassDependency 					dependencies {
		VK_SUBPASS_EXTERNAL,
		0,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		0,
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		0
	};

	// create the render pass
	VkRenderPassCreateInfo 					renderPassInfo {
		VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		nullptr,
		0,
		static_cast<uint32>(attachments.size()),
		attachments.data(),
		1,
		&subpassDescriptions,
		1,
		&dependencies
	};

	if(vkCreateRenderPass(device->get().device, &renderPassInfo, nullptr, &var.renderPass)) LOG_EXEPTION("Failed to create Vulkan render pass!")
}

void VulkanFramebuffers::create_frame_buffers() {
	var.framebuffers.resize(swapchain->get().images.images.size());

	for (size_t i = 0; i < swapchain->get().images.images.size(); i++) {
		VkImageView attachments[2] = {
			swapchain->get().images.images[i].view,
			swapchain->get().depthBuffer.image.view
		};

		// create the frame buffers
		VkFramebufferCreateInfo framebufferInfo{
			VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			nullptr,
			0,
			var.renderPass,
			2,
			attachments,
			swapchain->get().extent.width,
			swapchain->get().extent.height,
			1
		};

		if (vkCreateFramebuffer(device->get().device, &framebufferInfo, nullptr, &var.framebuffers[i]) != VK_SUCCESS) LOG_EXEPTION("Failed to create a framebuffer!");
	}
}

VkRenderPassBeginInfo VulkanFramebuffers::begin_info(const int index, const std::vector <VkClearValue> clear) const noexcept {
    return VkRenderPassBeginInfo {
		VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		nullptr,
		var.renderPass,
		var.framebuffers[index],
		{	// rendering area
			{0, 0},
			swapchain->get().extent
		},
		static_cast<uint32>(clear.size()),
		clear.data()
	};
}

VulkanFramebuffers::Variables VulkanFramebuffers::get() const noexcept {
	return var;
}

} // namespace lyra
