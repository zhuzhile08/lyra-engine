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
#include <core/rendering/window.h>
#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief a wrapper around the Vulkan instance
 */
class VulkanInstance {
public:
	VulkanInstance();

	/**
	 * @brief destructor of the instance
	 */
	virtual ~VulkanInstance() noexcept;

	/**
	 * @brief destroy the instance
	 */
	void destroy() noexcept;

	VulkanInstance operator=(const VulkanInstance&) const noexcept = delete;

	/**
	 * @brief create the instance and surface
	 *
	 * @param window window
	 */
	void create(const Window* const window);

	/**
	 * @brief get the Vulkan instance
	 * 
	 * @return const VkInstance
	 */
	[[nodiscard]] const VkInstance& instance() const noexcept { return _instance; }
	/**
	 * @brief get the Vulkan surface
	 *
	 * @return const VkSurfaceKHR
	 */
	[[nodiscard]] const VkSurfaceKHR& surface() const noexcept { return _surface; }

private:
	VkInstance _instance = VK_NULL_HANDLE;
	VkSurfaceKHR _surface = VK_NULL_HANDLE;

	const Window* window;

	/**
	 * @brief check if a vector of user requested Vulkan validation layers is actually available
	 *
	 * @param layers the available validation layers
	 * @param requestedLayers the requested validation layers
	 */
	void check_requested_validation_layers(const std::vector <VkLayerProperties> layers, const std::vector <const char*> requestedLayers) const;
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
