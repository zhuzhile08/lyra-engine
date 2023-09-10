#include <Graphics/ImGuiRenderer.h>

#include <Common/FileSystem.h>

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_sdl2.h>

namespace lyra {

ImGuiRenderer::ImGuiRenderer(const Window& window) : m_window(&window) {
	ImGui::CreateContext();
	m_io = &ImGui::GetIO();
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

void ImGuiRenderer::setIconFont(const std::filesystem::path& path, const ImFontConfig& fontConfig, const ImWchar* ranges, float size) {
	m_io->Fonts->AddFontDefault();
	m_io->Fonts->AddFontFromFileTTF(lyra::absolutePath(path).string().c_str(), size, &fontConfig, ranges);
}

void ImGuiRenderer::enableDocking() {
	m_io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

} // namespace lyra
