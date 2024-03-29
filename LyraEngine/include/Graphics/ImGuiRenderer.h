#pragma once

#include <Common/Common.h>
#include <Common/FunctionPointer.h>
#include <Common/UniquePointer.h>
#include <Common/RenderSystem.h>

#include <Graphics/SDLWindow.h>

#include <imgui.h>
#include <imgui_internal.h>

#include <filesystem>

namespace lyra {

class ImGuiRenderer : public VectorRenderSystem {
public:
	ImGuiRenderer() = default;
	ImGuiRenderer(const Window& window);
	virtual ~ImGuiRenderer();

	void draw();
	void setIconFont(const std::string& path, const ImFontConfig& fontConfig, const ImWchar* ranges, float32 size = 13.0f);
	void enableDocking();

	ImGuiIO& io() {
		return *m_io;
	}

protected:
	virtual void beginFrame() = 0;
	virtual void endFrame() = 0;

	ImGuiIO* m_io;
	const Window* m_window;
};

} // namespace lyra