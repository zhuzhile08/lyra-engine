/*************************
 * @file vulkan_window.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief wrapper around a Vulkan swapchain and related stuff
 *
 * @date 2022-02-05
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <core/decl.h>

#include <core/rendering/vulkan/vulkan_image.h>
#include <core/rendering/vulkan/GPU_memory.h>

#include <vector>
#include <algorithm>

#include <SDL.h>
#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief Wrapper around a Vulkan Swapchain
 */
class VulkanWindow {
public:
	VulkanWindow();

	/**
	 * @brief destructor of the swapchain
	 */
	virtual ~VulkanWindow() noexcept;
	/**
	 * @brief destroy the swapchain
	 */
	void destroy() noexcept {
		this->~VulkanWindow();
	}

	VulkanWindow operator=(const VulkanWindow&) const noexcept = delete;

	/**
	 * @brief create the swapchain
	 */
	void recreate();

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
	 * @brief get the Vulkan surface
	 *
	 * @return const VkSurfaceKHR&
	 */
	[[nodiscard]] const VkSurfaceKHR& surface() const noexcept { return _surface; }
	/**
	 * @brief get the swapchain
	 *
	 * @return const VkSwapchainKHR&
	*/
	[[nodiscard]] const VkSwapchainKHR& swapchain() const noexcept { return _swapchain; }
	/**
	 * @brief get the swapchain images
	 * 
	 * @return const std::vector<VkImage>&
	 */
	[[nodiscard]] const std::vector<VkImage>& images() const noexcept { return _images; }
	/**
	 * @brief get the image views of the swapchain images
	 *
	 * @return const std::vector<VkImageView>&
	 */
	[[nodiscard]] const std::vector<VkImageView>& views() const noexcept { return _views; }
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
	 * @brief get the color resources image
	 *
	 * @return const VulkanImage* const
	 */
	[[nodiscard]] const VulkanImage* const colorImage() const noexcept { return &_colorImage; }
	/**
	 * @brief get the maximum amout of samples
	 *
	 * @return const VkSampleCountFlagBits
	 */
	[[nodiscard]] const VkSampleCountFlagBits& maxMultisamples() const noexcept { return _maxMultisamples; }
	/**
	 * @brief get the depth buffer image
	 *
	 * @return const VulkanImage* const
	 */
	[[nodiscard]] const VulkanImage* const depthImage() const noexcept { return &_depthImage; }
	/**
	 * @brief get the depth buffer format
	 *
	 * @return const VkFormat&
	*/
	[[nodiscard]] const VkFormat& depthBufferFormat() const noexcept { return _depthBufferFormat; }
	/**
	 * @brief get the image available semaphores
	 *
	 * @return const std::vector <VkSemaphore>&
	*/
	[[nodiscard]] const std::vector <VkSemaphore>& imageAvailableSemaphores() const noexcept { return _imageAvailableSemaphores; }
	/**
	 * @brief get the render finished semaphores
	 *
	 * @return const std::vector <VkSemaphore>&
	*/
	[[nodiscard]] const std::vector <VkSemaphore>& renderFinishedSemaphores() const noexcept { return _renderFinishedSemaphores; }
	/**
	 * @brief get the in flight fences
	 *
	 * @return const std::vector <VkFence>&
	*/
	[[nodiscard]] const std::vector <VkFence>& inFlightFences() const noexcept { return _inFlightFences; }

private:
	VkSurfaceKHR _surface = VK_NULL_HANDLE;
	VkSwapchainKHR _swapchain = VK_NULL_HANDLE;
	VkFormat _format;
	VkExtent2D _extent;

	std::vector<VkImage> _images;
	std::vector<VkImageView> _views;
	
	VulkanImage _colorImage;
	VulkanGPUMemory _colorMem;
	VkSampleCountFlagBits _maxMultisamples = VK_SAMPLE_COUNT_1_BIT;

	VulkanImage _depthImage;
	VulkanGPUMemory _depthMem;
	VkFormat _depthBufferFormat;

	std::vector <VkSemaphore> _imageAvailableSemaphores;
	std::vector <VkSemaphore> _renderFinishedSemaphores;
	std::vector <VkFence> _inFlightFences;

	VkSwapchainKHR* _oldSwapchain = nullptr;

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
	 * @brief get the maximum amout of samples
	 *
	 * @return const VkSampleCountFlagBits
	 */
	[[nodiscard]] const VkSampleCountFlagBits get_max_samples() const noexcept;

	/**
	 * @brief check the correctness of the suface capabilities of the swapchain
	 *
	 * @param surfaceCapabilities the surface settings/capabilities to check
	*/
	void check_surface_capabilities(VkSurfaceCapabilitiesKHR& surfaceCapabilities) const;

	/**
	 * @brief create a extent of the swapchain
	 *
	 * @param surfaceCapabilities capabilities of the swapchain
	 */
	void create_swapchain_extent(const VkSurfaceCapabilitiesKHR surfaceCapabilities);

	/**
	 * @brief create a Vulkan window surface
	 */
	void create_window_surface();
	/**
	 * @brief create the swapchain images
	 */
	void create_swapchain_images();
	/**
	 * @brief create the depth buffer
	 */
	void create_depth_buffer();
	/**
	 * @brief create the color resources
	 */
	void create_color_resources();
	/**
	 * @brief create the swapchain
	 */
	void create_swapchain();
	/**
	 * @brief create the syncronisation objects
	 */
	void create_sync_objects();
};

} // namespace lyra
