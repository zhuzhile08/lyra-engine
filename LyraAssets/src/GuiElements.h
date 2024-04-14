/*************************
 * @file GuiElements.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Gui used in the asset manager
 * 
 * @date 2023-08-27
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include "SDLImGuiRenderer.h"
#include "ContentManager.h"

#include <Common/Common.h>
#include <Common/BasicRenderer.h>
#include <Common/Logger.h>

#include <Graphics/Window.h>
#include <Graphics/VulkanRenderSystem.h>

#include <filesystem>

struct ProgramState {
	ContentManager* contentManager;

	bool selected = false;
	bool opened = false;

	bool building = false;
	bool cleaning = false;

	bool rename = false;

	bool showProject = true;
	bool showProperties = true;
	bool showConsole = true;

	bool quit = false;

	std::string stringBuffer;
	std::filesystem::path nameBuffer;

	std::string logBuffer;
};

namespace gui {

class MainMenuBar : public lyra::RenderObject {
public:
	MainMenuBar(
		lyra::vulkan::ImGuiRenderer& renderer,
		ProgramState& state) : lyra::RenderObject(&renderer), m_state(&state) { }

	void draw() final;

private:
	ProgramState* m_state;
};

class ButtonBar : public lyra::RenderObject {
public:
	ButtonBar(
		lyra::vulkan::ImGuiRenderer& renderer,
		ProgramState& state) : lyra::RenderObject(&renderer), m_state(&state) { }

	void draw() final;

private:
	ProgramState* m_state;
};

class Window : public lyra::RenderObject {
public:
	Window(
		lyra::vulkan::ImGuiRenderer& renderer,
		ProgramState& state) : lyra::RenderObject(&renderer), m_state(&state) { }

	void draw() final;

private:
	ProgramState* m_state;
};

} // namespace gui
