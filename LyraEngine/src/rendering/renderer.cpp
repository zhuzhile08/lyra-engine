#include <rendering/renderer.h>

#include <core/array.h>

#include <core/queue.h>

#include <rendering/vulkan/devices.h>
#include <rendering/vulkan/frame.h>
#include <rendering/vulkan/command_buffer.h>
#include <rendering/vulkan/vulkan_window.h>

#include <rendering/render_system.h>

#include <core/application.h>

namespace lyra {

Renderer::Renderer() {
	// create the renderpass
	create_render_pass();
	
	// create the framebuffers
	create_framebuffers();

	// add the renderer to the render system
	Application::renderSystem.add_renderer(this);
}

void Renderer::recreate() {
	// destroy the framebuffers
	for (uint32 i = 0; i < m_framebuffers.size(); i++) m_framebuffers[i].destroy();
	// destroy the renderpass
	m_renderPass.destroy();
	create_render_pass();
	create_framebuffers();
}

void Renderer::create_render_pass() {
	// define what to do with an image during rendering
	VkAttachmentDescription colorAttachmentDescriptions{
		0,
		Application::renderSystem.vulkanWindow.format(),
		Application::renderSystem.vulkanWindow.maxMultisamples(),
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
		Application::renderSystem.vulkanWindow.depthBufferFormat(),
		Application::renderSystem.vulkanWindow.maxMultisamples(),
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		VK_ATTACHMENT_STORE_OP_DONT_CARE,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};

	// "finalise" the image to render
	VkAttachmentDescription colorAttachmentFinalDescriptions{ // Nanashi became Dagdas personal game engine programmer
		0,
		Application::renderSystem.vulkanWindow.format(),
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

	Array<VkAttachmentDescription, 3> attachments = {{ colorAttachmentDescriptions, depthBufferAttachmentDescriptions, colorAttachmentFinalDescriptions }};

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
	VkRenderPassCreateInfo createInfo{
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

	m_renderPass = vulkan::vk::RenderPass(Application::renderSystem.device.device(), createInfo);
}

void Renderer::create_framebuffers() {
	m_framebuffers.resize(Application::renderSystem.vulkanWindow.images().size());

	for (uint32 i = 0; i < Application::renderSystem.vulkanWindow.images().size(); i++) {
		Array<VkImageView, 3> attachments = { {
			Application::renderSystem.vulkanWindow.colorImage().view(),
			Application::renderSystem.vulkanWindow.depthImage().view(),
			Application::renderSystem.vulkanWindow.imageViews()[i]
		}};

		// create the frame buffers
		VkFramebufferCreateInfo createInfo{
			VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			nullptr,
			0,
			m_renderPass,
			static_cast<uint32>(attachments.size()),
			attachments.data(),
			Application::renderSystem.vulkanWindow.extent().width,
			Application::renderSystem.vulkanWindow.extent().height,
			1
		};

		m_framebuffers[i] = vulkan::vk::Framebuffer(Application::renderSystem.device.device(), createInfo);
	}
}

void Renderer::begin_renderpass() const {
	VkClearValue clear[2]{};
	clear[0].color = { {0.0f, 0.0f, 0.0f, 0.0f} };
	clear[1].depthStencil = { 1.0f, 0 };
	VkRenderPassBeginInfo beginInfo{
		VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		nullptr,
		m_renderPass,
		m_framebuffers[Application::renderSystem.imageIndex()],
		{	// rendering area
			{ 0, 0 },
			Application::renderSystem.vulkanWindow.extent()
		},
		2,
		clear
	};
	
	Application::renderSystem.frames[Application::renderSystem.currentFrame()].commandBuffer().beginRenderPass(beginInfo, VK_SUBPASS_CONTENTS_INLINE);
}  

void Renderer::end_renderpass() const { 
	Application::renderSystem.frames[Application::renderSystem.currentFrame()].commandBuffer().endRenderPass();
}

} // namespace lyra
