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

#include <core/settings.h>

#include <core/context.h>

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
	 * @return const float
	 */
	[[nodiscard]] const float fps() const noexcept { return _fps; }
	/**
	 * @brief get the deltatime
	 *
	 * @return const float
	 */
	[[nodiscard]] const float deltaTime() const noexcept { return _deltaTime; }

protected:
	float _fps;
	float _deltaTime;

	uint64 _lastTime;
	uint64 _currentTime;

	virtual void init() = 0;
};

} // namespace lyra