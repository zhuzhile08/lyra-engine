#include <core/rendering/renderer.h>

#include <array>

#include <core/logger.h>

#include <core/queue_types.h>

#include <core/rendering/vulkan/devices.h>
#include <core/rendering/vulkan/command_buffer.h>
#include <core/rendering/vulkan/vulkan_window.h>

#include <core/rendering/render_system.h>

#include <core/application.h>

namespace lyra {

Renderer::Renderer() {
	Logger::log_info("Creating Renderer...");

	// create the framebuffers
	create_render_pass();
	create_framebuffers();

	// add the renderer to the render system
	Application::renderSystem()->add_renderer(this);

	Logger::log_info("Successfully created Renderer at: ", get_address(this), "!");
}

Renderer::~Renderer() {
	// destrpy the framebuffer
	for (auto framebuffer : m_framebuffers) vkDestroyFramebuffer(Application::renderSystem()->m_device.device(), framebuffer, nullptr); // Yes, I've just probably broken some C++ convention rules or something, but since the context is a friend anyway, this should boost the performance by just a little bit
	vkDestroyRenderPass(Application::renderSystem()->m_device.device(), m_renderPass, nullptr);

	Logger::log_info("Successfully destroyed a renderer!");
}

void Renderer::recreate() {
	for (auto framebuffer : m_framebuffers) vkDestroyFramebuffer(Application::renderSystem()->m_device.device(), framebuffer, nullptr);
	vkDestroyRenderPass(Application::renderSystem()->m_device.device(), m_renderPass, nullptr);
	create_render_pass();
	create_framebuffers();
}

void Renderer::create_render_pass() {
	// define what to do with an image during rendering
	VkAttachmentDescription colorAttachmentDescriptions{
		0,
		Application::renderSystem()->m_vulkanWindow.format(),
		Application::renderSystem()->m_vulkanWindow.maxMultisamples(),
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
		Application::renderSystem()->m_vulkanWindow.depthBufferFormat(),
		Application::renderSystem()->m_vulkanWindow.maxMultisamples(),
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
		Application::renderSystem()->m_vulkanWindow.format(),
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

	vassert(vkCreateRenderPass(Application::renderSystem()->m_device.device(), &renderPassInfo, nullptr, &m_renderPass), "create Vulkan render pass");
}

void Renderer::create_framebuffers() {
	m_framebuffers.resize(Application::renderSystem()->m_vulkanWindow.images().size());

	for (int i = 0; i < Application::renderSystem()->m_vulkanWindow.images().size(); i++) {
		std::array<VkImageView, 3> attachments = {
			Application::renderSystem()->m_vulkanWindow.colorImage()->m_view,
			Application::renderSystem()->m_vulkanWindow.depthImage()->m_view,
			Application::renderSystem()->m_vulkanWindow.views().at(i)
		};

		// create the frame buffers
		VkFramebufferCreateInfo framebufferInfo{
			VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			nullptr,
			0,
			m_renderPass,
			static_cast<uint32>(attachments.size()),
			attachments.data(),
			Application::renderSystem()->m_vulkanWindow.extent().width,
			Application::renderSystem()->m_vulkanWindow.extent().height,
			1
		};

		vassert(vkCreateFramebuffer(Application::renderSystem()->m_device.device(), &framebufferInfo, nullptr, &m_framebuffers.at(i)), "create a framebuffer");
	}
}

void Renderer::begin_renderpass() const {
	VkClearValue clear[2]{};
	clear[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
	clear[1].depthStencil = { 1.0f, 0 };
	VkRenderPassBeginInfo beginInfo{
		VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		nullptr,
		m_renderPass,
		m_framebuffers.at(Application::renderSystem()->m_imageIndex),
		{	// rendering area
			{ 0, 0 },
			Application::renderSystem()->m_vulkanWindow.extent()
		},
		2, // hard coded
		clear
	};

	Application::renderSystem()->currentCommandBuffer().beginRenderPass(beginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void Renderer::end_renderpass() const { 
	Application::renderSystem()->currentCommandBuffer().endRenderPass();
}

} // namespace lyra
