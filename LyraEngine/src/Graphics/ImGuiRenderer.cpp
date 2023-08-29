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

void ImGuiRenderer::setIconFont(const std::filesystem::path& path, const ImFontConfig& fontConfig, const ImWchar* ranges, float size) {
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();
	io.Fonts->AddFontFromFileTTF(lyra::getGlobalPath(path).string().c_str(), size, &fontConfig, ranges);
}

} // namespace lyra
