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
#include <core/logger.h>
#include <lyra.h>

#include <array>
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
	 * @brief destructor of the framebuffers
	 */
	~VulkanFramebuffers() noexcept {
		for (auto framebuffer : _framebuffers) vkDestroyFramebuffer(device->device(), framebuffer, nullptr);
		vkDestroyRenderPass(device->device(), _renderPass, nullptr);

		Logger::log_info("Successfully destroyed Vulkan frame buffer!");
	}

	/**
	 * @brief destroy the framebuffers
	 */
	void destroy() noexcept {
		this->~VulkanFramebuffers();
	}

	VulkanFramebuffers operator=(const VulkanFramebuffers&) const noexcept = delete;

	/**
	 * @brief create the frame buffers and render pass
	 *
	 * @param device device
	 * @param swapchain swapchain
	 */
	void create(const VulkanDevice* const device, const VulkanSwapchain* const swapchain);

	/**
	 * @brief begin render pass
	*/
	void begin() const noexcept {
		VkClearValue clear[2] {};
		clear[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
		clear[1].depthStencil = { 1.0f, 0 };
		VkRenderPassBeginInfo beginInfo[]{
			VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			nullptr,
			_renderPass,
			_framebuffers.at(Application::context()->imageIndex()),
			{	// rendering area
				{ 0, 0 },
				swapchain->extent()
			},
			2, // hard coded
			clear
		};

		vkCmdBeginRenderPass(Application::context()->commandBuffers().at(Application::context()->currentFrame()).get(), beginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}
	/**
	 * @brief end render pass
	*/
	void end() const noexcept {
		vkCmdEndRenderPass(Application::context()->commandBuffers().at(Application::context()->currentFrame()).get());
	}

	/**
	 * @brief get the render pass
	 * 
	 * @return const VkRenderPass&
	*/
	[[nodiscard]] const VkRenderPass& renderPass() const noexcept { return _renderPass; }
	/**
	 * @brief get the framebuffers
	 * 
	 * @return const std::vector <VkFramebuffer>&
	*/
	[[nodiscard]] const std::vector <VkFramebuffer>& framebuffers() const noexcept { return _framebuffers; }

private:
	VkRenderPass _renderPass = VK_NULL_HANDLE;
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
