#include <rendering/renderer.h>

#include <core/array.h>

#include <core/queue.h>

#include <rendering/vulkan/devices.h>
#include <rendering/vulkan/command_buffer.h>
#include <rendering/vulkan/vulkan_window.h>

#include <rendering/render_system.h>

#include <core/application.h>

namespace lyra {

Renderer::Frame::~Frame() {
	vkDestroySemaphore(Application::renderSystem.device.device(), m_renderFinishedSemaphore, nullptr);
	vkDestroySemaphore(Application::renderSystem.device.device(), m_imageAvailableSemaphore, nullptr);
	vkDestroyFence(Application::renderSystem.device.device(), m_inFlightFence, nullptr);
}

void Renderer::Frame::wait() const {
	vassert(Application::renderSystem.device.waitForFence(m_inFlightFence, VK_TRUE, UINT64_MAX), "wait for Vulkan fences to finish");
}

void Renderer::Frame::reset() const {
	vassert(Application::renderSystem.device.resetFence(m_inFlightFence), "reset Vulkan fences");
}

void Renderer::Frame::create_sync_objects() {
	// semaphore create info
	VkSemaphoreCreateInfo semaphoreInfo{
		VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
	};

	// create both semaphores
	vassert(vkCreateSemaphore(Application::renderSystem.device.device(), &semaphoreInfo, nullptr, &m_imageAvailableSemaphore),
		"create Vulkan Synchronization Objects");
	vassert(vkCreateSemaphore(Application::renderSystem.device.device(), &semaphoreInfo, nullptr, &m_renderFinishedSemaphore),
		"create Vulkan Synchronization Objects");

	// fence create info
	VkFenceCreateInfo fenceInfo{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		nullptr,
		VK_FENCE_CREATE_SIGNALED_BIT
	};

	// create the fence
	vassert(vkCreateFence(Application::renderSystem.device.device(), &fenceInfo, nullptr, &m_inFlightFence),
		"create Vulkan Synchronization Objects");
}

Renderer::Renderer() {
	// create the renderpass
	create_render_pass();
	
	// create the virtual frames

	// add the renderer to the render system
	Application::renderSystem.add_renderer(this);
}

Renderer::~Renderer() {
	// destrpy the framebuffer
	vkDestroyRenderPass(Application::renderSystem.device.device(), m_renderPass, nullptr);
}

void Renderer::recreate() {
	vkDestroyRenderPass(Application::renderSystem.device.device(), m_renderPass, nullptr);
	create_render_pass();
	for (auto& frame : m_frames) frame.recreate();
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

	vassert(vkCreateRenderPass(Application::renderSystem.device.device(), &renderPassInfo, nullptr, &m_renderPass), "create Vulkan render pass");
}

void Renderer::create_framebuffers() {
	m_framebuffers.resize(Application::renderSystem.vulkanWindow.images().size());

	for (uint32 i = 0; i < Application::renderSystem.vulkanWindow.images().size(); i++) {
		Array<VkImageView, 3> attachments = {{
			Application::renderSystem.vulkanWindow.colorImage()->m_view,
			Application::renderSystem.vulkanWindow.depthImage()->m_view,
			Application::renderSystem.vulkanWindow.images().at(i).m_view
		}};

		// create the frame buffers
		VkFramebufferCreateInfo framebufferInfo{
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

		vassert(vkCreateFramebuffer(Application::renderSystem.device.device(), &framebufferInfo, nullptr, &m_framebuffers[i]), "create a framebuffer");
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
		m_frames.at(Application::renderSystem.m_imageIndex).m_framebuffer,
		{	// rendering area
			{ 0, 0 },
			Application::renderSystem.vulkanWindow.extent()
		},
		2,
		clear
	};

	Application::renderSystem.currentCommandBuffer.beginRenderPass(beginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void Renderer::end_renderpass() const { 
	Application::renderSystem.currentCommandBuffer.endRenderPass();
}

} // namespace lyra
