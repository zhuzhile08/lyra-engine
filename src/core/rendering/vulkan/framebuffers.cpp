#include <core/rendering/vulkan/framebuffers.h>

namespace lyra {

VulkanFramebuffers::VulkanFramebuffers() { }

void VulkanFramebuffers::create(const VulkanDevice* const device, const VulkanSwapchain* const swapchain) {
	Logger::log_info("Creating Vulkan framebuffers and render pass...");

	this->device = device;
	this->swapchain = swapchain;

	create_render_pass();
	create_frame_buffers();

	Logger::log_info("Successfully created Vulkan framebuffers and render pass at ", get_address(this), "!", Logger::end_l());
}

void VulkanFramebuffers::create_render_pass() {
	// define what to do with an image during rendering
	VkAttachmentDescription colorAttachmentDescriptions {
		0,
		swapchain->format(),
		swapchain->colorResources()->maxSamples(),
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	// depth buffers
	VkAttachmentDescription	depthBufferAttachmentDescriptions {
		0,
		swapchain->depthBuffer()->_format,
		swapchain->colorResources()->maxSamples(),
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};

	// "finalise" the image to render
	VkAttachmentDescription colorAttachmentFinalDescriptions { // Nanashi became Dagdas personal game engine programmer
		0,
		swapchain->format(),
		VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	};

	VkAttachmentReference colorAttachmentReferences {
		0,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	VkAttachmentReference depthBufferAttachmentReferences {
		1,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};

	VkAttachmentReference colorAttachmentFinalReferences{
		2,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	std::vector<VkAttachmentDescription> attachments = { colorAttachmentDescriptions, depthBufferAttachmentDescriptions, colorAttachmentFinalDescriptions };

	VkSubpassDescription subpassDescriptions {
		0,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		0,
		nullptr,
		1,
		&colorAttachmentReferences,
		&colorAttachmentFinalReferences,
		&depthBufferAttachmentReferences,
		0,
		nullptr
	};

	VkSubpassDependency dependencies {
		VK_SUBPASS_EXTERNAL,
		0,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		0,
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		0
	};

	// create the render pass
	VkRenderPassCreateInfo renderPassInfo {
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

	if (vkCreateRenderPass(device->device(), &renderPassInfo, nullptr, &_renderPass)) Logger::log_exception("Failed to create Vulkan render pass!");
}

void VulkanFramebuffers::create_frame_buffers() {
	_framebuffers.resize(swapchain->images()->_images.size());

	for (int i = 0; i < swapchain->images()->_images.size(); i++) {
		std::array<VkImageView, 3> attachments = {	
			swapchain->colorResources()->_view,
			swapchain->depthBuffer()->_view,
			swapchain->images()->_views.at(i)
		};

		// create the frame buffers
		VkFramebufferCreateInfo framebufferInfo {
			VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			nullptr,
			0,
			_renderPass,
			static_cast<uint32>(attachments.size()),
			attachments.data(),
			swapchain->extent().width,
			swapchain->extent().height,
			1
		};

		if (vkCreateFramebuffer(device->device(), &framebufferInfo, nullptr, &_framebuffers.at(i)) != VK_SUCCESS) Logger::log_exception("Failed to create a framebuffer!");
	}
}

} // namespace lyra
