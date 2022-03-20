/*************************
 * @file framebuffer.h
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

#include <memory>
#include <vector>

#include <SDL.h>
#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief Wrapper around Vulkan frame buffers
 */
class VulkanFramebuffers {
private:
	/**
	 * @brief struct containing all the variables
	 */
	struct 					Variables{
		VkRenderPass				renderPass;
		std::vector <VkFramebuffer> framebuffers;
	};

public:
	VulkanFramebuffers();

	/**
	 * @brief destroy the frame buffers
	 */
	void 					destroy();

	/**
	 * @brief create the render pass
	 * 
	 * @param device device
	 * @param swapchain swapchain
	 */
	void					create_render_pass(VulkanDevice device, VulkanSwapchain swapchain);

	/**
	 * @brief create the frame buffers
	 */
	void 					create();

	/**
	 * @brief get the begin information to begin a render pass
	 * 
	 * @param index index of framebuffer
	 * @param clear clear color
	 * @return VkRenderPassBeginInfo 
	 */
	VkRenderPassBeginInfo 	begin_info(const int index, const std::vector<VkClearValue> clear = { {CLEAR_COLOR}, {1.0f, 0} }) const;

	/**
	 * @brief get all the variabels
	 * 
	 * @return Variables
	 */
	Variables 				get() const;
	
private:
	Variables 				var;

	VulkanDevice*			device;
	VulkanSwapchain*    	swapchain;
};

} // namespace lyra
