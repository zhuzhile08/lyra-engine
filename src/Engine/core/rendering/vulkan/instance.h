/*************************
 * @file instance.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief a wrapper around a Vulkan instance
 *
 * @date 2022-02-04
 *
 * @copyright Copyright (c) 2022 Zhile Zhu
 *************************/

#pragma once

#include <core/defines.h>
#include <core/logger.h>

#include <SDL.h>
#include <SDL_vulkan.h>
#include <graphics/window.h>
#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief a wrapper around the Vulkan instance
 */
class VulkanInstance {
public:
	VulkanInstance();

	/**
	 * @brief destroy a Instance
	 */
	void destroy() noexcept;

	/**
	 * @brief create the instance and surface
	 *
	 * @param window window
	 */
	void create(const Window* window);

	/**
	 * @brief get the Vulkan instance
	 * 
	 * @return const VkInstance
	 */
	const VkInstance instance() const noexcept { return _instance; }
	/**
	 * @brief get the Vulkan surface
	 *
	 * @return const VkSurfaceKHR
	 */
	const VkSurfaceKHR surface() const noexcept { return _surface; }

private:
	VkInstance _instance;
	VkSurfaceKHR _surface;

	const Window* window;

	/**
	 * @brief check if a vector of user requested Vulkan validation layers is actually available
	 *
	 * @param layers the available validation layers
	 * @param requestedLayers the requested validation layers
	 */
	void check_requested_validation_layers(const std::vector <VkLayerProperties> layers, const std::vector <str> requestedLayers) const;
	/**
	 * @brief create a Vulkan instance
	 *
	 * @param window window
	 */
	void create_instance();

	/**
	 * @brief create a Vulkan window surface
	 *
	 * @param window window
	 */
	void create_window_surface();
};

} // namespace lyra
