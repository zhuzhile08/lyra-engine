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

#include <lyra.h>

#include <core/dynarray.h>
#include <vector>

#include <vulkan/vulkan.h>

#include <core/settings.h>

#include <rendering/vulkan/GPU_memory.h>
#include <rendering/vulkan/vulkan_image.h>

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

	Window operator=(const Window&) const noexcept = delete;

	/**
	 * @brief create the swapchain
	 */
	void recreate();

	/**
	 * @brief get the Vulkan surface
	 *
	 * @return constexpr VkSurfaceKHR
	 */
	NODISCARD constexpr VkSurfaceKHR surface() const noexcept { return m_surface; }
	/**
	 * @brief get the swapchain
	 *
	 * @return constexpr const VkSwapchainKHR&
	*/
	NODISCARD constexpr const VkSwapchainKHR& swapchain() const noexcept { return m_swapchain; }
	/**
	 * @brief get the swapchain images
	 *
	 * @return const lyra::Dynarray<VkImage, lyra::vulkan::Window::maxSwapchainImages>
	 */
	NODISCARD const Dynarray<VkImage, Settings::RenderConfig::maxSwapchainImages> images() const noexcept { return m_images; }
	/**
	 * @brief get the swapchain images
	 *
	 * @return const lyra::Dynarray<VkImageView, lyra::vulkan::Window::maxSwapchainImages>
	 */
	NODISCARD const Dynarray<VkImageView, Settings::RenderConfig::maxSwapchainImages> imageViews() const noexcept { return m_imageViews; }
	/**
	 * @brief get the depth buffer image
	 *
	 * @return constexpr const Image* const
	 */
	NODISCARD constexpr const Image* const depthImage() const noexcept { return &m_depthImage; }
	/**
	 * @brief get the color resources image
	 *
	 * @return constexpr const Image* const
	 */
	NODISCARD constexpr const Image* const colorImage() const noexcept { return &m_colorImage; }
	/**
	 * @brief get the format
	 *
	 * @return constexpr VkFormat
	*/
	NODISCARD constexpr VkFormat format() const noexcept { return m_format; }
	/**
	 * @brief get the extent
	 *
	 * @return constexpr VkExtent2D
	*/
	NODISCARD constexpr VkExtent2D extent() const noexcept { return m_extent; }
	/**
	 * @brief get the maximum amout of samples
	 *
	 * @return constexpr VkSampleCountFlagBits
	 */
	NODISCARD constexpr VkSampleCountFlagBits maxMultisamples() const noexcept { return m_maxMultisamples; }
	/**
	 * @brief get the depth buffer format
	 *
	 * @return constexpr VkFormat&
	*/
	NODISCARD constexpr VkFormat depthBufferFormat() const noexcept { return m_depthBufferFormat; }

private:

	VkSurfaceKHR m_surface;
	VkSwapchainKHR m_swapchain;
	VkFormat m_format;
	VkExtent2D m_extent;

	Dynarray<VkImage, Settings::RenderConfig::maxSwapchainImages> m_images;
	Dynarray<VkImageView, Settings::RenderConfig::maxSwapchainImages> m_imageViews;
	
	Image m_colorImage;
	GPUMemory m_colorMem;
	VkSampleCountFlagBits m_maxMultisamples = VK_SAMPLE_COUNT_1_BIT;

	Image m_depthImage;
	GPUMemory m_depthMem;
	VkFormat m_depthBufferFormat;

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
	NODISCARD const VkPresentModeKHR get_optimal_present_mode() const;
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
	void create_swapchain_extent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities);

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
};

} // namespace vulkan

} // namespace lyra
