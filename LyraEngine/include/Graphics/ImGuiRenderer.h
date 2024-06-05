#pragma once

#include <Common/Common.h>
#include <LSD/FunctionPointer.h>
#include <LSD/UniquePointer.h>
#include <Common/BasicRenderer.h>

#include <Graphics/Window.h>

#include <imgui.h>
#include <imgui_internal.h>

#include <filesystem>

namespace lyra {

class ImGuiRenderer : public VectorBasicRenderer {
public:
	ImGuiRenderer();
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
};

} // namespace lyra