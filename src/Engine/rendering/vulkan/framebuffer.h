/*************************
 * @file framebuffer.h
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
#include <rendering/vulkan/devices.h>
#include <rendering/vulkan/swapchain.h>
#include <core/logger.h>

#include <memory>
#include <vector>

#include <SDL.h>
#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief Wrapper around Vulkan frame buffers
 */
class VulkanFramebuffers {
public:
	VulkanFramebuffers();

	/**
	 * @brief destroy the frame buffers and render pass
	 */
	void destroy() noexcept;

	/**
	 * @brief create the frame buffers and render pass
	 *
	 * @param device device
	 * @param swapchain swapchain
	 */
	void create(const VulkanDevice* device, const VulkanSwapchain* swapchain);

	/**
	 * @brief get the begin information to begin a render pass
	 *
	 * @param index index of framebuffer
	 * @param clear clear color
	 * @return VkRenderPassBeginInfo
	 */
	VkRenderPassBeginInfo get_begin_info(const int index, const std::vector<VkClearValue> clear = { {{CLEAR_COLOR}}, {1.0f, 0} }) const noexcept;

	/**
	 * @brief get the render pass
	 * 
	 * @return const VkRenderPass
	*/
	const VkRenderPass renderPass() const noexcept { return _renderPass; }
	/**
	 * @brief get the framebuffers
	 * 
	 * @return const std::vector <VkFramebuffer>
	*/
	const std::vector <VkFramebuffer> framebuffers() const noexcept { return _framebuffers; }

private:
	VkRenderPass _renderPass;
	std::vector <VkFramebuffer> _framebuffers;

	const VulkanDevice* device;
	const VulkanSwapchain* swapchain;

	/**
	 * @brief create the render pass
	 */
	void create_render_pass();

	/**
	 * @brief create the frame buffers
	 */
	void create_frame_buffers();
};

} // namespace lyra
