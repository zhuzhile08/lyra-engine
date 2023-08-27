/*************************
 * @file SDLImGuiRenderer.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief 
 * 
 * @date 2023-08-27
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <Common/Common.h>

#include <Graphics/ImGuiRenderer.h>

#include <SDL_render.h>

class SDLImGuiRenderer : public lyra::ImGuiRenderer {
public:
	SDLImGuiRenderer() = default;
	SDLImGuiRenderer(const lyra::Window& window);
	~SDLImGuiRenderer();

private:
	void beginFrame() final;
	void endFrame() final;

    SDL_Renderer* m_renderer;
};
