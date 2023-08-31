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

#include <Common/Common.h>
#include <Common/RenderSystem.h>
#include <Graphics/SDLWindow.h>

#include <filesystem>

struct ProgramState {
	bool selected = false;
	bool opened = false;

	bool building = false;
	bool cleaning = false;

	bool unsaved = false;
	bool rename = false;

	std::string nameBuffer;

	bool* running = nullptr;
};

namespace gui {

class MainMenuBar : public lyra::RenderObject {
public:
	MainMenuBar(
		lyra::Window& window, 
		SDLImGuiRenderer& renderer,
		ProgramState& state) : lyra::RenderObject(&renderer), m_state(&state) { }

	void draw() final;

private:
	ProgramState* m_state;
};

class ButtonBar : public lyra::RenderObject {
public:
	ButtonBar(
		SDLImGuiRenderer& renderer,
		ProgramState& state) : lyra::RenderObject(&renderer), m_state(&state) { }

	void draw() final;

private:
	ProgramState* m_state;
};

} // namespace gui
