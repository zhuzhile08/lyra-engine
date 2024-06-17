#include <Graphics/ImGuiRenderer.h>

#include <Common/FileSystem.h>

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_sdl3.h>

namespace lyra {

ImGuiRenderer::ImGuiRenderer() {
	ImGui::CreateContext();
	m_io = &ImGui::GetIO();
}

ImGuiRenderer::~ImGuiRenderer() {
	ImGui::DestroyContext();
}

void ImGuiRenderer::draw() {
	beginFrame();

	drawAll();

	endFrame();
}

void ImGuiRenderer::setIconFont(const lsd::String& path, const ImFontConfig& fontConfig, const ImWchar* ranges, float32 size) {
	m_io->Fonts->AddFontDefault();
	m_io->Fonts->AddFontFromFileTTF(lyra::absolutePath(path.cStr()).string().c_str(), size, &fontConfig, ranges);
}

void ImGuiRenderer::enableDocking() {
	m_io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

} // namespace lyra
