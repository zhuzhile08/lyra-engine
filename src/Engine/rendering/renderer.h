/*************************
 * @file renderer.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief a rendering system around the Vulkan API with basic features
 *
 * @date 2022-02-04
 *
 * @copyright Copyright (c) 2022 Zhile Zhu
 *************************/

#pragma once

#include <core/defines.h>
#include <core/queue_types.h>
#include <rendering/vulkan/command_buffer.h>
#include <rendering/vulkan/descriptor.h>
#include <rendering/vulkan/devices.h>
#include <rendering/vulkan/sync_objects.h>
#include <rendering/vulkan/instance.h>
#include <rendering/vulkan/swapchain.h>
#include <rendering/window.h>

#include <core/logger.h>

#include <memory>
#include <vector>

#include <noud.h>
#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief a vulkan renderer with basic features
 * @todo maybe abstract vma memory allocation?
 */
class Renderer : noud::Node {
public:
	Renderer();
	/**
	 * @brief destroy the renderer
	 */
	void destroy() noexcept;
	/**
	 * @brief create the renderer
	 *
	 * @param window the window
	 */
	void create(const Window* window);

	/**
	 * @brief the main loop. this is called every frame until drawing is set to false
	 */
	void update();

	/**
	 * @brief submit a Vulkan queue after command queue recording
	 *
	 * @param queue the queue to submit
	 * @param commandBuffer recorded commandbuffer
	 * @param stageFlags pipeline shader stage flags
	 */
	void submit_device_queue(const VulkanDevice::VulkanQueueFamily queue, const VulkanCommandBuffer commandBuffer, const VkPipelineStageFlags stageFlags) const;

	/**
	 * @brief wait for queue to finish submitting
	 *
	 * @param queue queue to wait for
	 */
	void wait_device_queue(const VulkanDevice::VulkanQueueFamily queue) const;

	/**
	 * @brief get the device
	 * 
	 * @return const VulkanDevice
	*/
	const VulkanDevice device() const noexcept { return _device; }
	/**
	 * @brief get the command pool
	 * 
	 * @return const VulkanCommandPool
	*/
	const VulkanCommandPool commandPool() const noexcept { return _commandPool; }
	/**
	 * @brief get the swapchain
	 * 
	 * @return const VulkanSwapchain
	*/
	const VulkanSwapchain swapchain() const noexcept { return _swapchain; }
	/**
	 * @brief get the descriptor set layout
	 * 
	 * @return const VulkanDescriptorSetLayout
	*/
	const VulkanDescriptorSetLayout descriptorSetLayout() const noexcept { return _descriptorSetLayout; }
	/**
	 * @brief get the descriptor pool
	 * 
	 * @return const VulkanDescriptorPool
	*/
	const VulkanDescriptorPool descriptorPool() const noexcept { return _descriptorPool; }
	/**
	 * @brief get the current frame count
	 * 
	 * @return const uint8
	*/
	const uint8 currentFrame() const noexcept { return _currentFrame; }
	/**
	 * @brief get the image index
	 * 
	 * @return const uint32
	*/
	const uint32 imageIndex() const noexcept { return _imageIndex; }

private:
	VulkanInstance _instance;
	VulkanDevice _device;
	VulkanCommandPool _commandPool;
	VulkanSwapchain _swapchain;
	VulkanDescriptorSetLayout _descriptorSetLayout;
	VulkanDescriptorPool _descriptorPool;
	VulkanSyncObjects _syncObjects;

	CallQueue _renderQueue;
	CallQueue _submitQueue;

	bool _running = true;

	uint8 _currentFrame = 0;
	uint32 _imageIndex;

	const Window* window;

	/**
	 * @brief recreate the swapchain and related stuff in case of some events
	 */
	void recreate_swapchain();
	/**
	 * @brief destroy the swapchain and related stuff in case of some events
	 */
	void destroy_swapchain() noexcept;

	/**
	 * @brief present all the recorded commands
	 *
	 * @param index of the images
	 */
	void present_queue(const uint32 imageIndex);
	/**
	 * @brief take the recorded commands and draw everything
	 */
	void draw();
	/**
	 * @brief update the frame count
	 */
	void update_frame_count() noexcept;

	friend class RenderStage;
};

} // namespace Vulkan
