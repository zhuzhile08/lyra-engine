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

namespace gui {

class MainMenuBar : public lyra::RenderObject {
public:
	MainMenuBar(lyra::Window& window, SDLImGuiRenderer& renderer) : lyra::RenderObject(&renderer), m_running(&window.running()) { }

	void draw() final;

private:
	bool* m_running;
};

class ButtonBar : public lyra::RenderObject {
public:
	ButtonBar(SDLImGuiRenderer& renderer) : lyra::RenderObject(&renderer) { }

	void draw() final;
};

} // namespace gui
