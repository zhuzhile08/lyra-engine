/*************************
 * @file   context.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief  wrapper around the core components of the engine
 * 
 * @date   2022-30-7
 * 
 * @copyright Copyright (c) 2022
*************************/

#pragma once

#include <core/singleton.h>

#include <core/rendering/render_system.h>
#include <core/rendering/window.h>

namespace lyra {

class Context : public Singleton<Context> {
public:
	/**
	 * @brief construct the context
	 */
	Context() : _window(), _renderSystem(&_window) { }

	/**
	 * @brief get the window
	 * 
	 * @return const lyra::Window* const
	 */
	[[nodiscard]] Window* const window() noexcept { return &_window; }
	/**
	 * @brief get the render system
	 * 
	 * @return const lyra::RenderSystem* const
	 */
	[[nodiscard]] RenderSystem* const renderSystem() noexcept { return &_renderSystem; }

private:
	Window _window;
	RenderSystem _renderSystem;
};

} // namespace lyra
