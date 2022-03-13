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
#include <rendering/vulkan/instance.h>
#include <core/logger.h>

#include <memory>
#include <vector>

#include <SDL.h>
#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief wrapper around swapchain images
 */
struct VulkanSwapchainImages {
public:
	VulkanSwapchainImages();

	/**
	 * @brief destroy the VulkanSwapchainImages
	 */
	void destroy();
	
	/**
	 * @brief create the image views
	 * 
	 * @param device device
	 * @param format swapchain format
	 * @param swapchain swapchain
	 */
	void						create(VulkanDevice device, const VkSurfaceFormatKHR format, const VkSwapchainKHR swapchain);

	std::vector <VkImage>       images;
	std::vector <VkImageView>   views;

	VulkanDevice*				device;
};

/**
 * @brief Wrapper around a Vulkan Swapchain
 */
class VulkanSwapchain {
private:
	/**
	 * @brief struct containing all the variables
	 */
	struct Variables {
		VkSwapchainKHR  			swapchain;
		VkFormat        			format;
		VkExtent2D          		extent;
		VulkanSwapchainImages 		images;

		VulkanSwapchain*			oldSwapchain = nullptr;
	};

public:
	VulkanSwapchain();

	/**
	 * @brief destroy the VulkanSwapchain
	 */
	void 						destroy();

	/**
	 * @brief create the swapchain
	 * 
	 * @param device device
	 * @param instance instance
	 * @param window window
	 */
	void 						create(VulkanDevice device, VulkanInstance instance, Window window);

	/**
	 * @brief create the swapchain
	 * 
	 * @param device device
	 * @param instance instance
	 * @param window window
	 * @param oldSwapchain old swapchain
	 */
	void 						create(VulkanDevice device, VulkanInstance instance, Window window, VulkanSwapchain oldSwapchain);

	/**
	 * @brief get all variables
	 * 
	 * @return Variables
	 */
	Variables  					get() const;
	
private:
	Variables 					var;

	VulkanDevice*				device;
	VulkanInstance*				instance;
	Window* 					window;

	/**
	 * @brief create the swapchain
	 */
	void 						create_swapchain();
	/**
	 * @brief create a extent of the swapchain 
	 *
	 * @param surfaceCapabilities capabilities of the swapchain
	 */
	void 						create_swapchain_extent(VkSurfaceCapabilitiesKHR *surfaceCapabilities);
};

} // namespace lyra
