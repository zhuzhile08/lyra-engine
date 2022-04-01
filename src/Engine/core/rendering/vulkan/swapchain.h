/*************************
 * @file swapchain.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief wrapper around a Vulkan swapchain and related stuff
 *
 * @date 2022-02-05
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <core/defines.h>
#include <core/rendering/vulkan/devices.h>
#include <core/rendering/vulkan/instance.h>
#include <core/rendering/vulkan/vulkan_image.h>
#include <core/rendering/vulkan/command_buffer.h>
#include <core/rendering/vulkan/GPU_memory.h>
#include <core/logger.h>

#include <vector>

#include <SDL.h>
#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief Wrapper around a Vulkan Swapchain
 */
class VulkanSwapchain {
private:
	/**
	 * @brief wrapper around swapchain images
	 */
	struct VulkanSwapchainImages : std::vector<VulkanImage> {
	public:
		VulkanSwapchainImages();

		/**
		 * @brief destroy the swapchain images
		 */
		void destroy() noexcept;

		/**
		 * @brief create the image views
		 *
		 * @param device device
		 * @param swapchain swapchain
		 */
		void create(const VulkanDevice* device, VulkanSwapchain swapchain);

	private:
		const VulkanDevice* device;
	};

	/**
	 * @brief wrapper around depth buffers
	 */
	struct VulkanDepthBuffer : VulkanImage, VulkanGPUMemory {
	public:
		VulkanDepthBuffer();

		/**
		 * @brief destroy the depth buffer
		 */
		void destroy() noexcept;

		/**
		 * @brief create the image, view and allocate the memory
		 *
		 * @param device device
		 * @param swapchain swapchain
		 * @param cmdPool command pool
		 */
		void create(const VulkanDevice* device, const VulkanSwapchain swapchain, const VulkanCommandPool cmdPool);

		VkFormat _format;

	private:
		const VulkanDevice* device;
	};

public:
	VulkanSwapchain();

	/**
	 * @brief destroy the VulkanSwapchain
	 */
	void destroy() noexcept;

	/**
	 * @brief create the swapchain
	 *
	 * @param device device
	 * @param cmdPool command pool
	 * @param instance instance
	 * @param window window
	 */
	void create(const VulkanDevice* device, const VulkanInstance* instance, const VulkanCommandPool cmdPool, const Window* window);

	/**
	 * @brief create the swapchain
	 *
	 * @param oldSwapchain old swapchain
	 */
	void create(VulkanSwapchain oldSwapchain, const VulkanCommandPool cmdPool);

	/**
	 * @brief get the swapchain
	 *
	 * @return const VkSwapchainKHR
	*/
	const VkSwapchainKHR swapchain() const noexcept { return _swapchain; }
	/**
	 * @brief get the format
	 *
	 * @return const VkFormat
	*/
	const VkFormat format() const noexcept { return _format; }
	/**
	 * @brief get the extent
	 *
	 * @return const VkExtent2D
	*/
	const VkExtent2D extent() const noexcept { return _extent; }
	/**
	 * @brief get the swapchain images
	 *
	 * @return const VulkanSwapchainImages
	*/
	const VulkanSwapchainImages images() const noexcept { return _images; }
	/**
	 * @brief get the depth buffer
	 *
	 * @return const VulkanDepthBuffer
	*/
	const VulkanDepthBuffer depthBuffer() const noexcept { return _depthBuffer; }

private:
	VkSwapchainKHR _swapchain;
	VkFormat _format;
	VkExtent2D _extent;
	VulkanSwapchainImages _images;
	VulkanDepthBuffer _depthBuffer;

	VulkanSwapchain* _oldSwapchain = nullptr;

	const VulkanDevice* device;
	const VulkanInstance* instance;
	const Window* window;

	/**
	 * @brief create the swapchain
	 * 
	 * @param cmdPool command pool
	 */
	void create_swapchain(const VulkanCommandPool cmdPool);
	/**
	 * @brief create a extent of the swapchain
	 *
	 * @param surfaceCapabilities capabilities of the swapchain
	 */
	void create_swapchain_extent(VkSurfaceCapabilitiesKHR* surfaceCapabilities);
};

} // namespace lyra
