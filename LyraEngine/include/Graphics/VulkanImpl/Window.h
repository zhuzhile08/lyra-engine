/*************************
 * @file Window.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief wrapper around a Vulkan swapchain and related stuff
 *
 * @date 2022-02-05
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <Lyra/Lyra.h>

#include <Common/Dynarray.h>
#include <vector>

#include <vulkan/vulkan.h>

#include <Common/Settings.h>

#include <Graphics/VulkanImpl/GPUMemory.h>
#include <Graphics/VulkanImpl/Image.h>

namespace lyra {

namespace vulkan {

/**
 * @brief Wrapper around a Vulkan Swapchain
 */
class Window {
public:
	Window();

	/**
	 * @brief create the swapchain
	 */
	void recreate();

	/**
	 * @brief get the Vulkan surface
	 *
	 * @return constexpr lyra::vulkan::vk::Surface&
	 */
	NODISCARD constexpr const vk::SurfaceKHR& surface() const noexcept { return m_surface; }
	/**
	 * @brief get the swapchain
	 *
	 * @return constexpr const lyra::vulkan::vk::SwapchainKHR&
	*/
	NODISCARD constexpr const vk::Swapchain& swapchain() const noexcept { return m_swapchain; }
	/**
	 * @brief get the swapchain images
	 *
	 * @return const lyra::Dynarray<lyra::vulkan::vk::Image, lyra::vulkan::Window::maxSwapchainImages>&
	 */
	NODISCARD const Dynarray<vk::Image, Settings::RenderConfig::maxSwapchainImages>& images() const noexcept { return m_images; }
	/**
	 * @brief get the swapchain images
	 *
	 * @return const lyra::Dynarray<yra::vulkan::vk::ImageView, lyra::vulkan::Window::maxSwapchainImages>&
	 */
	NODISCARD const Dynarray<vk::ImageView, Settings::RenderConfig::maxSwapchainImages>& imageViews() const noexcept { return m_imageViews; }
	/**
	 * @brief get the depth buffer image
	 *
	 * @return constexpr const lyra::vulkan::Image& const
	 */
	NODISCARD constexpr const Image& depthImage() const noexcept { return m_depthImage; }
	/**
	 * @brief get the color resources image
	 *
	 * @return constexpr const lyra::vulkan::Image& const
	 */
	NODISCARD constexpr const Image& colorImage() const noexcept { return m_colorImage; }
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
	 * @return constexpr VkFormat
	*/
	NODISCARD constexpr VkFormat depthBufferFormat() const noexcept { return m_depthBufferFormat; }

private:
	vk::Surface m_surface;
	vk::Swapchain m_swapchain;

	VkFormat m_format;
	VkExtent2D m_extent;

	Dynarray<vk::Image, Settings::RenderConfig::maxSwapchainImages> m_images;
	Dynarray<vk::ImageView, Settings::RenderConfig::maxSwapchainImages> m_imageViews;
	
	Image m_colorImage;
	GPUMemory m_colorMem;
	VkSampleCountFlagBits m_maxMultisamples = VK_SAMPLE_COUNT_1_BIT;

	Image m_depthImage;
	GPUMemory m_depthMem;
	VkFormat m_depthBufferFormat;

	vk::Swapchain* m_oldSwapchain = nullptr;

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
