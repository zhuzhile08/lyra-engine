#include <Graphics/ImGuiRenderer.h>

#include <Common/FileSystem.h>

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_sdl2.h>

namespace lyra {

ImGuiRenderer::ImGuiRenderer(const Window& window) : m_window(&window) {
	ImGui::CreateContext();
}

ImGuiRenderer::~ImGuiRenderer() {
	ImGui::DestroyContext();
}

void ImGuiRenderer::draw() {
	beginFrame();
	ImGui::NewFrame();

	drawAll();

	ImGui::Render();
	endFrame();
}

void ImGuiRenderer::addFont(const std::filesystem::path& path, const ImFontConfig& fontConfig, const std::vector<ImWchar>& ranges, float size) {
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF(lyra::getGlobalPath(path).c_str(), size, &fontConfig, ranges.data());
}

namespace gui {

/*
ImGuiRenderer::ImGuiRenderer() : Renderer() {
	// information about the descriptor pool
	vulkan::DescriptorSystem::PoolBuilder builder;
	builder.addPool_sizes({
		{ vulkan::DescriptorSystem::DescriptorSet::Type::sampler, 500 },
		{ vulkan::DescriptorSystem::DescriptorSet::Type::imageSampler, 500 },
		{ vulkan::DescriptorSystem::DescriptorSet::Type::sampledImage, 500 },
		{ vulkan::DescriptorSystem::DescriptorSet::Type::storageImage, 500 },
		{ vulkan::DescriptorSystem::DescriptorSet::Type::uniformTexelBuffer, 500 },
		{ vulkan::DescriptorSystem::DescriptorSet::Type::storageTexelBuffer, 500 },
		{ vulkan::DescriptorSystem::DescriptorSet::Type::uniformBuffer, 500 },
		{ vulkan::DescriptorSystem::DescriptorSet::Type::storageBuffer, 500 },
		{ vulkan::DescriptorSystem::DescriptorSet::Type::uniformBufferDynamic, 500 },
		{ vulkan::DescriptorSystem::DescriptorSet::Type::storageBufferDynamic, 500 },
		{ vulkan::DescriptorSystem::DescriptorSet::Type::inputAttachment, 500 }
	});
	builder.setPoolFlags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);
	// create the descriptor pool
	m_descriptorPool = UniquePointer<vulkan::DescriptorSystem::DescriptorPool>::create(builder.build_CreateInfo());

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
		Application::renderSystem.device.graphicsQueue(),
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
	cmdBuff.submitQueue(Application::renderSystem.device.graphicsQueue());
	// reset the command buffer
	cmdBuff.reset();

	// destroy font data after creating
	ImGui_ImplVulkan_DestroyFontUploadObjects();
}

ImGuiRenderer::~ImGuiRenderer() {
	ImGui_ImplVulkan_Shutdown();
}

void ImGuiRenderer::addDrawCall(std::function<void()>&& func) {
	m_drawQueue.add(std::move(func));
}

void ImGuiRenderer::record_command_buffers() {
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
*/

} // namespace gui

} // namespace lyra
