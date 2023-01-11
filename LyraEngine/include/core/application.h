/*************************
 * @file core/application.h
 *
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief wrapper the basic application features
 *
 * @date 2022-04-30
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <lyra.h>

#include <rendering/render_system.h>
#include <rendering/window.h>

namespace lyra {

// @brief wrapper around a every basic component of an application
class Application {
public:
	/**
	 * @brief construct a new application
	 */
	Application();

	/**
	 * @brief destroy the application
	 */
	~Application();

	Application(const Application&) noexcept = delete;
	Application operator=(const Application&) const noexcept = delete;

	/**
	 * @brief quit the application
	 */
	void quit();

	/**
	 * @brief draw the current frame
	*/
	void draw();

	/**
	 * @brief get the fps
	 *
	 * @return constexpr float
	 */
	NODISCARD constexpr float fps() const noexcept { return 1000.0f / (float)(m_currentTime - m_lastTime); }
	/**
	 * @brief get the deltatime
	 *
	 * @return const float
	 */
	NODISCARD constexpr float deltaTime() const noexcept { return (float)(m_currentTime - m_lastTime); }
	
	static Window window;
	static RenderSystem renderSystem;
	
protected:
	uint64 m_lastTime;
	uint64 m_currentTime;

	virtual void init() { }
};

} // namespace lyra