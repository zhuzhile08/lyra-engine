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
	NODISCARD constexpr float fps() const noexcept { return m_fps; }
	/**
	 * @brief get the deltatime
	 *
	 * @return const float
	 */
	NODISCARD constexpr float deltaTime() const noexcept { return m_deltaTime; }
	/**
	 * @brief get the window
	 *
	 * @return constexpr lyra::Window* const
	 */
	NODISCARD constexpr static Window* const window() noexcept { return &m_window; }
	/**
	 * @brief get the render system
	 *
	 * @return constexpr lyra::RenderSystem* const
	 */
	NODISCARD constexpr static RenderSystem* const renderSystem() noexcept { return &m_renderSystem; }
	
protected:
	float m_fps;
	float m_deltaTime;

	uint64 m_lastTime;
	uint64 m_currentTime;

	static Window m_window;
	static RenderSystem m_renderSystem;

	virtual void init() { }
};

/**
 * @brief initialize lyra
 */
void init(void);

} // namespace lyra