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

#include <vector>

#include <vulkan/vulkan.h>

#include <core/decl.h>

#include <core/rendering/vulkan/GPU_memory.h>
#include <core/rendering/vulkan/vulkan_image.h>

namespace lyra {

namespace vulkan {

/**
 * @brief Wrapper around a Vulkan Swapchain
 */
class Window {
public:
	Window();

	/**
	 * @brief destructor of the swapchain
	 */
	virtual ~Window() noexcept;
	/**
	 * @brief destroy the swapchain
	 */
	void destroy() noexcept {
		this->~Window();
	}

	Window operator=(const Window&) const noexcept = delete;

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
	NODISCARD const VkSurfaceKHR& surface() const noexcept { return m_surface; }
	/**
	 * @brief get the swapchain
	 *
	 * @return const VkSwapchainKHR&
	*/
	NODISCARD const VkSwapchainKHR& swapchain() const noexcept { return m_swapchain; }
	/**
	 * @brief get the swapchain images
	 * 
	 * @return const std::vector<VkImage>&
	 */
	NODISCARD const std::vector<VkImage>& images() const noexcept { return m_images; }
	/**
	 * @brief get the image views of the swapchain images
	 *
	 * @return const std::vector<VkImageView>&
	 */
	NODISCARD const std::vector<VkImageView>& views() const noexcept { return m_views; }
	/**
	 * @brief get the format
	 *
	 * @return const VkFormat&
	*/
	NODISCARD const VkFormat& format() const noexcept { return m_format; }
	/**
	 * @brief get the extent
	 *
	 * @return const VkExtent2D&
	*/
	NODISCARD const VkExtent2D& extent() const noexcept { return m_extent; }
	/**
	 * @brief get the color resources image
	 *
	 * @return const Image* const
	 */
	NODISCARD const Image* const colorImage() const noexcept { return &m_colorImage; }
	/**
	 * @brief get the maximum amout of samples
	 *
	 * @return const VkSampleCountFlagBits
	 */
	NODISCARD const VkSampleCountFlagBits& maxMultisamples() const noexcept { return m_maxMultisamples; }
	/**
	 * @brief get the depth buffer image
	 *
	 * @return const Image* const
	 */
	NODISCARD const Image* const depthImage() const noexcept { return &m_depthImage; }
	/**
	 * @brief get the depth buffer format
	 *
	 * @return const VkFormat&
	*/
	NODISCARD const VkFormat& depthBufferFormat() const noexcept { return m_depthBufferFormat; }
	/**
	 * @brief get the image available semaphores
	 *
	 * @return const std::vector <VkSemaphore>&
	*/
	NODISCARD const std::vector <VkSemaphore>& imageAvailableSemaphores() const noexcept { return m_imageAvailableSemaphores; }
	/**
	 * @brief get the render finished semaphores
	 *
	 * @return const std::vector <VkSemaphore>&
	*/
	NODISCARD const std::vector <VkSemaphore>& renderFinishedSemaphores() const noexcept { return m_renderFinishedSemaphores; }
	/**
	 * @brief get the in flight fences
	 *
	 * @return const std::vector <VkFence>&
	*/
	NODISCARD const std::vector <VkFence>& inFlightFences() const noexcept { return m_inFlightFences; }

private:
	VkSurfaceKHR m_surface = VK_NULL_HANDLE;
	VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
	VkFormat m_format;
	VkExtent2D m_extent;

	std::vector<VkImage> m_images;
	std::vector<VkImageView> m_views;
	
	Image m_colorImage;
	GPUMemory m_colorMem;
	VkSampleCountFlagBits m_maxMultisamples = VK_SAMPLE_COUNT_1_BIT;

	Image m_depthImage;
	GPUMemory m_depthMem;
	VkFormat m_depthBufferFormat;

	std::vector <VkSemaphore> m_imageAvailableSemaphores;
	std::vector <VkSemaphore> m_renderFinishedSemaphores;
	std::vector <VkFence> m_inFlightFences;

	VkSwapchainKHR* m_oldSwapchain = nullptr;

	/**
	 * @brief get the optimal format for the swapchain
	 * 
	 * @return const VkSurfaceFormatKHR
	*/
	NODISCARD const VkSurfaceFormatKHR get_optimal_format();
	/**
	 * @brief get the optimal presentation mode for the swapchain
	 *
	 * @return const VkPresentModeKHR
	*/
	NODISCARD const VkPresentModeKHR get_optimal_present_mode();
	/**
	 * @brief get the maximum amout of samples
	 *
	 * @return const VkSampleCountFlagBits
	 */
	NODISCARD const VkSampleCountFlagBits get_max_samples() const noexcept;

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

} // namespace vulkan

} // namespace lyra
