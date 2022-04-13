/*************************
 * @file sync_objects.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief wrapper around Vulkan semaphores and fences to synchronise GPU and CPU
 *
 * @date 2022-02-05
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <core/defines.h>
#include <core/rendering/vulkan/devices.h>
#include <core/rendering/vulkan/swapchain.h>
#include <core/logger.h>

#include <vector>

#include <SDL.h>
#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief semaphores and fences for frame CPU and GPU synchronisation
 */
class VulkanSyncObjects {
public:
	VulkanSyncObjects();

	/**
	 * @brief destroy the VulkanSyncObjects
	 * @brief it works just like the destructor, but is callable anywhere
	 */
	void destroy() noexcept;

	/**
	 * @brief create the syncronisation objects
	 *
	 * @param device device
	 */
	void create(const VulkanDevice* device);

	/**
	 * @brief wait for Vulkan fences
	 *
	 * @param fenceIndex index of the fence
	 */
	void wait(const uint32 fenceIndex) const;
	/**
	 * @brief reset Vulkan fences
	 *
	 * @param fenceIndex index of fence to reset
	 */
	void reset(const uint32 imageIndex) const;

	/**
	 * @brief get the image available semaphores
	 *
	 * @return const std::vector <VkSemaphore>
	*/
	const std::vector <VkSemaphore> imageAvailableSemaphores() const noexcept { return _imageAvailableSemaphores; }
	/**
	 * @brief get the render finished semaphores
	 *
	 * @return const std::vector <VkSemaphore>
	*/
	const std::vector <VkSemaphore> renderFinishedSemaphores() const noexcept { return _renderFinishedSemaphores; }
	/**
	 * @brief get the in flight fences
	 *
	 * @return const std::vector <VkFence>
	*/
	const std::vector <VkFence> inFlightFences() const noexcept { return _inFlightFences; }

private:
	std::vector <VkSemaphore> _imageAvailableSemaphores;
	std::vector <VkSemaphore> _renderFinishedSemaphores;
	std::vector <VkFence> _inFlightFences;

	const VulkanDevice* device;
};

} // namespace lyra
