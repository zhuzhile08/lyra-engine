/*************************
 * @file swapchain.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief wrapper around a Vulkan swapchain and related stuff
 * 
 * @date 2022-02-05
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <core/defines.h>
#include <rendering/vulkan/devices.h>
#include <rendering/vulkan/swapchain.h>
#include <core/logger.h>

#include <vector>

#include <SDL.h>
#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief semaphores and fences for frame CPU and GPU synchronisation
 */
class VulkanSyncObjects {
private:
	struct 			Variables {
		std::vector <VkSemaphore> 	imageAvailableSemaphores;
		std::vector <VkSemaphore>   renderFinishedSemaphores;
		std::vector <VkFence> 		inFlightFences;
		std::vector <VkFence> 		imagesInFlight;
		size_t 						currentFrame = 0;
	};

public:
	VulkanSyncObjects();

	/**
	 * @brief destroy the VulkanSyncObjects
	 * @brief it works just like the destructor, but is callable anywhere
	 */
	void 			destroy();

	/**
	 * @brief create the syncronisation objects
	 * 
	 * @param device device
	 * @param swapchain swapchain
	 */
	void 			create(VulkanDevice device, const VulkanSwapchain swapchain);

	/**
	 * @brief wait for Vulkan fences
	 * 
	 * @param fenceIndex index of the fence
	 */
	void 			wait(const uint32 fenceIndex);
	/**
	 * @brief reset Vulkan fences
	 * 
	 * @param fenceIndex index of fence to reset
	 */
	void 			reset(const uint32 imageIndex);

	/**
	 * @brief get all the variabels
	 * 
	 * @return Variables
	 */
	Variables 		get() const;
	/**
	 * @brief Set the current frame
	 * 
	 * @param newFrame the frame to set
	 */
	void			set_current_frame(const size_t newFrame);

private:
	Variables 		var;

	VulkanDevice*	device;
};

} // namespace lyra
