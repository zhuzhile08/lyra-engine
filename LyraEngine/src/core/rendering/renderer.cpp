#include <core/rendering/renderer.h>

#include <core/logger.h>

#include <core/queue_types.h>

#include <core/rendering/vulkan/devices.h>
#include <core/rendering/vulkan/command_buffer.h>
#include <core/rendering/vulkan/vulkan_window.h>
#include <core/rendering/vulkan/GPU_buffer.h>
#include <core/rendering/render_system.h>

namespace lyra {

Renderer::Renderer() {
	Logger::log_info("Creating Renderer...");

	create_render_pass();
	create_framebuffers();

	Context::get()->renderSystem()->add_renderer(this);

	Logger::log_info("Successfully created Renderer at: ", get_address(this), "!");
}

Renderer::~Renderer() noexcept {
	for (auto framebuffer : _framebuffers) vkDestroyFramebuffer(Context::get()->renderSystem()->_device->device(), framebuffer, nullptr); // Yes, I've just probably broken some C++ convention rules or something, but since the context is a friend anyway, this should boost the performance by just a little bit
	vkDestroyRenderPass(Context::get()->renderSystem()->_device->device(), _renderPass, nullptr);

	Logger::log_info("Successfully destroyed a renderer!");
}

void Renderer::recreate() {
	for (auto framebuffer : _framebuffers) vkDestroyFramebuffer(Context::get()->renderSystem()->_device->device(), framebuffer, nullptr);
	vkDestroyRenderPass(Context::get()->renderSystem()->_device->device(), _renderPass, nullptr);
	create_render_pass();
	create_framebuffers();
}

void Renderer::add_to_draw_queue(std::function<void()>&& function) {
	_drawQueue.add(std::move(function));
}

void Renderer::add_to_update_queue(std::function<void()>&& function) {
	_updateQueue.add(std::move(function));
}

void Renderer::create_render_pass() {
	// define what to do with an image during rendering
	VkAttachmentDescription colorAttachmentDescriptions{
		0,
		Context::get()->renderSystem()->_vulkanWindow->format(),
		Context::get()->renderSystem()->_vulkanWindow->maxMultisamples(),
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	// depth buffers
	VkAttachmentDescription	depthBufferAttachmentDescriptions{
		0,
		Context::get()->renderSystem()->_vulkanWindow->depthBufferFormat(),
		Context::get()->renderSystem()->_vulkanWindow->maxMultisamples(),
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};

	// "finalise" the image to render
	VkAttachmentDescription colorAttachmentFinalDescriptions{ // Nanashi became Dagdas personal game engine programmer
		0,
		Context::get()->renderSystem()->_vulkanWindow->format(),
		VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	};

	VkAttachmentReference colorAttachmentReferences{
		0,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	VkAttachmentReference depthBufferAttachmentReferences{
		1,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};

	VkAttachmentReference colorAttachmentFinalReferences{
		2,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	std::array<VkAttachmentDescription, 3> attachments = { colorAttachmentDescriptions, depthBufferAttachmentDescriptions, colorAttachmentFinalDescriptions };

	VkSubpassDescription subpassDescriptions{
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

	VkSubpassDependency dependencies{
		VK_SUBPASS_EXTERNAL,
		0,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		0,
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		0
	};

	// create the render pass
	VkRenderPassCreateInfo renderPassInfo{
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

	lassert(vkCreateRenderPass(Context::get()->renderSystem()->_device->device(), &renderPassInfo, nullptr, &_renderPass) == VK_SUCCESS, "Failed to create Vulkan render pass!");
}

void Renderer::create_framebuffers() {
	_framebuffers.resize(Context::get()->renderSystem()->_vulkanWindow->images().size());

	for (int i = 0; i < Context::get()->renderSystem()->_vulkanWindow->images().size(); i++) {
		std::array<VkImageView, 3> attachments = {
			Context::get()->renderSystem()->_vulkanWindow->colorImage()->_view,
			Context::get()->renderSystem()->_vulkanWindow->depthImage()->_view,
			Context::get()->renderSystem()->_vulkanWindow->views().at(i)
		};

		// create the frame buffers
		VkFramebufferCreateInfo framebufferInfo{
			VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			nullptr,
			0,
			_renderPass,
			static_cast<uint32>(attachments.size()),
			attachments.data(),
			Context::get()->renderSystem()->_vulkanWindow->extent().width,
			Context::get()->renderSystem()->_vulkanWindow->extent().height,
			1
		};

		lassert(vkCreateFramebuffer(Context::get()->renderSystem()->_device->device(), &framebufferInfo, nullptr, &_framebuffers.at(i)) == VK_SUCCESS, "Failed to create a framebuffer!");
	}
}

void Renderer::record_command_buffers() const {
	VkClearValue clear[2]{};
	clear[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
	clear[1].depthStencil = { 1.0f, 0 };
	VkRenderPassBeginInfo beginInfo{
		VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		nullptr,
		_renderPass,
		_framebuffers.at(Context::get()->renderSystem()->_imageIndex),
		{	// rendering area
			{ 0, 0 },
			Context::get()->renderSystem()->_vulkanWindow->extent()
		},
		2, // hard coded
		clear
	};

	vkCmdBeginRenderPass(Context::get()->renderSystem()->activeCommandBuffer(), &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

	_drawQueue.flush();

	vkCmdEndRenderPass(Context::get()->renderSystem()->activeCommandBuffer());
}

} // namespace lyra
