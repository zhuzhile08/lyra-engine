#include <core/rendering/vulkan/framebuffers.h>

namespace lyra {

VulkanFramebuffers::VulkanFramebuffers() { }

VulkanFramebuffers::~VulkanFramebuffers() noexcept {
	for (auto framebuffer : _framebuffers) vkDestroyFramebuffer(device->device(), framebuffer, nullptr);
	vkDestroyRenderPass(device->device(), _renderPass, nullptr);

	Logger::log_info("Succesfully destroyed Vulkan frame buffer!");
}

void VulkanFramebuffers::destroy() noexcept {
	this->~VulkanFramebuffers();
}

void VulkanFramebuffers::create(const VulkanDevice* const device, const VulkanSwapchain* const swapchain) {
	Logger::log_info("Creating Vulkan framebuffers and render pass...");

	this->device = device;
	this->swapchain = swapchain;

	create_render_pass();
	create_frame_buffers();

	Logger::log_info("Succesfully created Vulkan framebuffers and render pass at ", get_address(this), "!", Logger::end_l());
}

void VulkanFramebuffers::create_render_pass() {
	// define what to do with an image during rendering
	VkAttachmentDescription imageAttachmentDescriptions {
		0,
		swapchain->format(),
		VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	};

	// depth buffers
	VkAttachmentDescription	depthBufferAttachmentDescriptions { // first error, depth buffer gets destroyed here
		0,
		swapchain->depthBuffer()->_format,
		VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};

	VkAttachmentReference colorAttachmentReferences {
		0,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	VkAttachmentReference depthBufferAttachmentReferences {
		1,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};

	std::vector<VkAttachmentDescription> attachments = { imageAttachmentDescriptions, depthBufferAttachmentDescriptions };

	VkSubpassDescription subpassDescriptions {
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
		std::array<VkImageView, 2> attachments = {
			swapchain->images()->_views.at(i),
			swapchain->depthBuffer()->_view
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

const VkRenderPassBeginInfo VulkanFramebuffers::get_begin_info(const int index, const std::array<VkClearValue, 2> clear) const noexcept {
	return {
		VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		nullptr,
		_renderPass,
		_framebuffers.at(index),
		{	// rendering area
			{ 0, 0 },
			swapchain->extent()
		},
		static_cast<uint32>(clear.size()),
		clear.data()
	};
}

} // namespace lyra
