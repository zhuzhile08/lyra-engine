#include <Graphics/GUIRenderer.h>

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_sdl2.h>

#include <Common/Queue.h>

#include <Graphics/VulkanImpl/Device.h>
#include <Graphics/VulkanImpl/CommandBuffer.h>
#include <Graphics/VulkanImpl/Window.h>

#include <Graphics/Renderer.h>

#include <Application/Application.h>

namespace lyra {

namespace gui {

GUIRenderer::GUIRenderer() : Renderer() {
	// information about the descriptor pool
	vulkan::DescriptorSystem::PoolBuilder builder;
	builder.add_pool_sizes({
		{ vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_SAMPLER, 500 },
		{ vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_IMAGE_SAMPLER, 500 },
		{ vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_SAMPLED_IMAGE, 500 },
		{ vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_STORAGE_IMAGE, 500 },
		{ vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_UNIFORM_TEXEL_BUFFER, 500 },
		{ vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_STORAGE_TEXEL_BUFFER, 500 },
		{ vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_UNIFORM_BUFFER, 500 },
		{ vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_STORAGE_BUFFER, 500 },
		{ vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_UNIFORM_BUFFER_DYNAMIC, 500 },
		{ vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_STORAGE_BUFFER_DYNAMIC, 500 },
		{ vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_INPUT_ATTACHMENT, 500 }
	});
	builder.set_pool_flags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);
	// create the descriptor pool
	m_descriptorPool = SmartPointer<vulkan::DescriptorSystem::DescriptorPool>::create(builder.build_create_info());

	// initialize ImGui
	ImGui::CreateContext();
	// initialize ImGui for SDL
	ImGui_ImplSDL2_InitForVulkan(Application::window.get());
	// initialization information
	ImGui_ImplVulkan_InitInfo initInfo{
		Application::renderSystem.device.instance(),
		Application::renderSystem.device.physicalDevice(),
		Application::renderSystem.device.device(),
		Application::renderSystem.device.graphicsQueue().familyIndex,
		Application::renderSystem.device.graphicsQueue().queue,
		VK_NULL_HANDLE,
		m_descriptorPool->get(),
		0,
		3,
		3,
		Application::renderSystem.vulkanWindow.maxMultisamples()
	};
	// initialize ImGui for Vulkan
	ImGui_ImplVulkan_Init(&initInfo, m_renderPass);

	// get a command buffer for creating the font textures
	vulkan::CommandBuffer cmdBuff(vulkan::CommandBuffer::Usage::USAGE_ONE_TIME_SUBMIT);
	// start recording the command buffer
	cmdBuff.begin();
	// create the textures
	ImGui_ImplVulkan_CreateFontsTexture(cmdBuff.commandBuffer());
	// end recording the command buffer
	cmdBuff.end();
	// submit the commands
	cmdBuff.submitQueue(Application::renderSystem.device.graphicsQueue().queue);
	// reset the command buffer
	cmdBuff.reset();

	// destroy font data after creating
	ImGui_ImplVulkan_DestroyFontUploadObjects();
}

GUIRenderer::~GUIRenderer() {
	ImGui_ImplVulkan_Shutdown();
}

void GUIRenderer::add_draw_call(std::function<void()>&& func) {
	m_drawQueue.add(std::move(func));
}

void GUIRenderer::record_command_buffers() {
	begin_renderpass();

	// begin drawing
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplSDL2_NewFrame(Application::window.get());
	ImGui::NewFrame();

	// flush all draw commands
	m_drawQueue.flush();

	// render
	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), Application::renderSystem.frames[Application::renderSystem.currentFrame()].commandBuffer().commandBuffer());

	end_renderpass();
}

} // namespace gui

} // namespace lyra
