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
#include <algorithm>

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
	struct VulkanSwapchainImages {
	public:
		VulkanSwapchainImages() { }

		/**
		 * @brief destructor of the swapchain images
		 */
		virtual ~VulkanSwapchainImages() noexcept {
			for (auto& view : _views) vkDestroyImageView(device->device(), view, nullptr);

			Logger::log_info("Successfully destroyed Vulkan swapchain images!");
		}

		/**
		 * @brief destroy the swapchain images
		 */
		void destroy() noexcept {
			this->~VulkanSwapchainImages();
		}

		VulkanSwapchainImages operator=(const VulkanSwapchainImages&) const noexcept = delete;

		/**
		 * @brief create the image views
		 *
		 * @param device device
		 * @param swapchain swapchain
		 */
		void create(const VulkanDevice* const device, const VulkanSwapchain* const swapchain);

		std::vector<VkImage> _images;
		std::vector<VkImageView> _views;

	private:
		const VulkanDevice* device;
	};

	/**
	 * @brief wrapper around depth buffers
	 */
	struct VulkanDepthBuffer : VulkanImage, VulkanGPUMemory {
	public:
		VulkanDepthBuffer() { }

		/**
		 * @brief destroy the depth buffer
		 */
		void destroy() noexcept {
			this->~VulkanDepthBuffer();
		}

		VulkanDepthBuffer operator=(const VulkanDepthBuffer&) const noexcept = delete;

		/**
		 * @brief create the image, view and allocate the memory
		 *
		 * @param device device
		 * @param swapchain swapchain
		 * @param cmdPool command pool
		 */
		void create(const VulkanDevice* const device, const VulkanSwapchain* const swapchain, const VulkanCommandPool* const cmdPool);

		VkFormat _format;

	private:
		const VulkanDevice* device;
	};

public:
	VulkanSwapchain() { }

	/**
	 * @brief destructor of the swapchain
	 */
	virtual ~VulkanSwapchain() noexcept {
		vkDestroySwapchainKHR(device->device(), _swapchain, nullptr);
		if (_oldSwapchain != nullptr) vkDestroySwapchainKHR(device->device(), *_oldSwapchain, nullptr);

		Logger::log_info("Successfully destroyed Vulkan swapchain!");
	}

	/**
	 * @brief destroy the swapchain
	 */
	void destroy() noexcept {
		this->~VulkanSwapchain();
	}

	VulkanSwapchain operator=(const VulkanSwapchain&) const noexcept = delete;

	/**
	 * @brief create the swapchain
	 *
	 * @param device device
	 * @param cmdPool command pool
	 * @param instance instance
	 * @param window window
	 */
	void create(const VulkanDevice* const device, const VulkanInstance* const instance, const VulkanCommandPool* const cmdPool, const Window* const window);

	/**
	 * @brief create the swapchain
	 */
	void recreate();

	/**
	 * @brief get the swapchain
	 *
	 * @return const VkSwapchainKHR&
	*/
	[[nodiscard]] const VkSwapchainKHR& swapchain() const noexcept { return _swapchain; }
	/**
	 * @brief get the format
	 *
	 * @return const VkFormat&
	*/
	[[nodiscard]] const VkFormat& format() const noexcept { return _format; }
	/**
	 * @brief get the extent
	 *
	 * @return const VkExtent2D&
	*/
	[[nodiscard]] const VkExtent2D& extent() const noexcept { return _extent; }
	/**
	 * @brief get the swapchain images
	 *
	 * @return const lyra::VulkanSwapchainImages*
	*/
	[[nodiscard]] const VulkanSwapchainImages* images() const noexcept { return &_images; }
	/**
	 * @brief get the depth buffer
	 *
	 * @return const lyra::VulkanDepthBuffer*
	*/
	[[nodiscard]] const VulkanDepthBuffer* depthBuffer() const noexcept { return &_depthBuffer; }

private:
	VkSwapchainKHR _swapchain = VK_NULL_HANDLE;
	VkFormat _format;
	VkExtent2D _extent;
	VulkanSwapchainImages _images;
	VulkanDepthBuffer _depthBuffer;

	VkSwapchainKHR* _oldSwapchain = nullptr;

	const VulkanDevice* device;
	const VulkanInstance* instance;
	const VulkanCommandPool* cmdPool;
	const Window* window;

	/**
	 * @brief get the optimal format for the swapchain
	 * 
	 * @return const VkSurfaceFormatKHR
	*/
	[[nodiscard]] const VkSurfaceFormatKHR get_optimal_format();
	/**
	 * @brief get the optimal presentation mode for the swapchain
	 *
	 * @return const VkPresentModeKHR
	*/
	[[nodiscard]] const VkPresentModeKHR get_optimal_present_mode();
	/**
	 * @brief check the correctness of the suface capabilities of the swapchain
	 *
	 * @param surfaceCapabilities the surface settings/capabilities to check
	*/
	void check_surface_capabilities(VkSurfaceCapabilitiesKHR& surfaceCapabilities) const;

	/**
	 * @brief create the swapchain
	 *
	 * @param cmdPool command pool
	 */
	void create_swapchain(const VulkanCommandPool* const cmdPool);
	/**
	 * @brief create a extent of the swapchain
	 *
	 * @param surfaceCapabilities capabilities of the swapchain
	 */
	void create_swapchain_extent(const VkSurfaceCapabilitiesKHR surfaceCapabilities);
};

} // namespace lyra
