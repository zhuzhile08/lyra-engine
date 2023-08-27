#pragma once

#include <Common/Common.h>
#include <Common/FunctionPointer.h>
#include <Common/UniquePointer.h>
#include <Common/RenderSystem.h>

#include <Graphics/SDLWindow.h>

#include <imgui_internal.h>

#include <filesystem>

namespace lyra {

class ImGuiRenderer : public UnorderedRenderSystem, public RenderObject {
public:
	ImGuiRenderer() = default;
	ImGuiRenderer(const Window& window);
	virtual ~ImGuiRenderer();

	void draw();
	void addFont(const std::filesystem::path& path, const ImFontConfig& fontConfig, const std::vector<ImWchar>& ranges, float size = 13.0f);

	ImGuiViewport& getViewport() {
		return *ImGui::GetMainViewport();
	}
	ImGuiIO& getIO() {
		return ImGui::GetIO();
	}

protected:
	virtual void beginFrame() = 0;
	virtual void endFrame() = 0;

	const Window* m_window;
};

namespace gui {

} // namespace gui

} // namespace lyra