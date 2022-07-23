#include <core/rendering/renderer.h>

#include <core/logger.h>

#include <core/queue_types.h>

#include <core/rendering/vulkan/devices.h>
#include <core/rendering/vulkan/command_buffer.h>
#include <core/rendering/vulkan/vulkan_window.h>
#include <core/rendering/vulkan/GPU_buffer.h>
#include <core/rendering/context.h>

namespace lyra {

Renderer::Renderer() {
	Logger::log_info("Creating Renderer...");

	_drawQueue = new CallQueue;
	_updateQueue = new CallQueue;
	create_render_pass();
	create_framebuffers();

	const_cast<Context*>(Application::context())->add_to_recreate_queue(FUNC_PTR(
		for (auto framebuffer : _framebuffers) vkDestroyFramebuffer(Application::context()->device()->device(), framebuffer, nullptr);
		vkDestroyRenderPass(Application::context()->device()->device(), _renderPass, nullptr);
		create_render_pass();
		create_framebuffers();
	));

	Logger::log_info("Successfully created Renderer at: ", get_address(this), "!");
}

Renderer::~Renderer() noexcept {
	delete _drawQueue;
	delete _updateQueue;
	for (auto framebuffer : _framebuffers) vkDestroyFramebuffer(Application::context()->device()->device(), framebuffer, nullptr);
	vkDestroyRenderPass(Application::context()->device()->device(), _renderPass, nullptr);

	Logger::log_info("Successfully destroyed a renderer!");
}

void Renderer::bind() noexcept {
	Application::context()->add_to_update_queue([&]() { _updateQueue->flush(); });
	Application::context()->add_to_render_queue([&]() { record_command_buffers(); });
}

void Renderer::add_to_draw_queue(std::function<void()>&& function) {
	_drawQueue->add(std::move(function));
}

void Renderer::add_to_update_queue(std::function<void()>&& function) {
	_updateQueue->add(std::move(function));
}

void Renderer::create_render_pass() {
	// define what to do with an image during rendering
	VkAttachmentDescription colorAttachmentDescriptions{
		0,
		Application::context()->vulkanWindow()->format(),
		Application::context()->vulkanWindow()->maxMultisamples(),
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
		Application::context()->vulkanWindow()->depthBufferFormat(),
		Application::context()->vulkanWindow()->maxMultisamples(),
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
		Application::context()->vulkanWindow()->format(),
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

	lassert(vkCreateRenderPass(Application::context()->device()->device(), &renderPassInfo, nullptr, &_renderPass) == VK_SUCCESS, "Failed to create Vulkan render pass!");
}

void Renderer::create_framebuffers() {
	_framebuffers.resize(Application::context()->vulkanWindow()->images().size());

	for (int i = 0; i < Application::context()->vulkanWindow()->images().size(); i++) {
		std::array<VkImageView, 3> attachments = {
			Application::context()->vulkanWindow()->colorImage()->_view,
			Application::context()->vulkanWindow()->depthImage()->_view,
			Application::context()->vulkanWindow()->views().at(i)
		};

		// create the frame buffers
		VkFramebufferCreateInfo framebufferInfo{
			VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			nullptr,
			0,
			_renderPass,
			static_cast<uint32>(attachments.size()),
			attachments.data(),
			Application::context()->vulkanWindow()->extent().width,
			Application::context()->vulkanWindow()->extent().height,
			1
		};

		lassert(vkCreateFramebuffer(Application::context()->device()->device(), &framebufferInfo, nullptr, &_framebuffers.at(i)) == VK_SUCCESS, "Failed to create a framebuffer!");
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
		_framebuffers.at(Application::context()->imageIndex()),
		{	// rendering area
			{ 0, 0 },
			Application::context()->vulkanWindow()->extent()
		},
		2, // hard coded
		clear
	};

	vkCmdBeginRenderPass(Application::context()->commandBuffers()->commandBuffer(Application::context()->currentCommandBuffer())->commandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

	_drawQueue->flush();

	vkCmdEndRenderPass(Application::context()->commandBuffers()->commandBuffer(Application::context()->currentCommandBuffer())->commandBuffer);
}

} // namespace lyra
