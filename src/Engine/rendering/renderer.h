/*************************
 * @file rendering.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * s
 * @brief a rendering system around the Vulkan API with basic features
 * 
 * @date 2022-02-04
 * 
 * @copyright Copyright (c) 2022 Zhile Zhu
 *************************/

#pragma once

#include <core/defines.h>
#include <res/loaders/load_file.h>
#include <res/mesh.h>
#include <rendering/vulkan_wrappers.h>
#include <rendering/window.h>

#include <core/logger.h>

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

namespace lyra { 

/**
 * @brief a vulkan renderer with basic features
 * @todo maybe abstract vma memory allocation?
 */
class Renderer {
public:
	Renderer();
	/**
	 * @brief destroy the renderer
	 */
	void 								destroy();
	/**
	 * @brief create the renderer
	 * 
	 * @param window the window
	 */
	void 								create(Window window);

	/**
	 * @brief recreate the swapchain and related stuff in case of some events
	 */
	void 								recreateSwapchain();
	/**
	 * @brief destroy the swapchain and related stuff in case of some events
	 */
	void 								destroySwapchain();

	/// @todo queue and multithread this function
	void load_scene();
	void unload_scene();
	/// @todo not sure how to do this, but output the drawn renders as pictures as efficiently as possible

	void update();
	/// @todo also queue and multithread this function
	void draw();
	

private:
	VulkanInstance						instance;
	VulkanDevice						device;
	VulkanCommandPool					commandPool;
	VulkanSwapchain 					swapchain;
	VulkanFramebuffers 					framebuffers;
	VulkanDescriptorSetLayout 			descriptorSetLayout;
	/// @todo textures samplers, and other stuff to do
	VulkanDescriptorPool				descriptorPool;
	std::vector<VulkanDescriptor>		descriptors;
	std::vector<VulkanCommandBuffer>	commandBuffers;
	VulkanSyncObjects 					syncObjects;

	Window* 							window;

	/**
	 * @brief bind a descriptor set
	 * 
	 * @param descriptor descriptor
	 * @param pipelineLayout pipeline layout
	 */
	void 								bind_descriptor(const VulkanDescriptor descriptor, const VulkanGraphicsPipeline pipeline, const int cmdBuffIndex) const;
	void 								push_constants() const;
	/**
	 * @brief begin render passes
	 * 
	 * @param framebuffers frame buffer
	 * @param index index of the frame buffer
	 */
	void 								begin_render_pass(const VulkanFramebuffers framebuffer, const int index, const VkClearValue clear, const int cmdBuffIndex) const;
	/**
	 * @brief end render passes
	 * 
	 * @param framebuffers frame buffer
	 */
	void								end_render_pass(const int cmdBuffIndex) const;
	/**
	 * @brief bind the graphics pipeline
	 * 
	 * @param pipeline pipeline
	 */
	void 								bind_pipeline(const VulkanGraphicsPipeline pipeline, const int cmdBuffIndex) const;
	void 								bind_model() const;
	void								draw_model() const;
};

} // namespace Vulkan
