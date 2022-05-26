/*************************
 * @file context.h
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
#include <core/logger.h>
#include <core/queue_types.h>
#include <core/rendering/vulkan/command_buffer.h>
#include <core/rendering/vulkan/descriptor.h>
#include <core/rendering/vulkan/devices.h>
#include <core/rendering/vulkan/sync_objects.h>
#include <core/rendering/vulkan/instance.h>
#include <core/rendering/vulkan/swapchain.h>
#include <core/rendering/window.h>

#include <vector>

#include <noud.h>
#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief a vulkan context containing everything needed for an application
 */
class Context {
public:
	Context();

	/**
	 * @brief destructor of the context
	 */
	virtual ~Context() noexcept;

	/**
	 * @brief destroy the context
	 */
	void destroy() noexcept;

	Context(const Context&) noexcept = delete;
	Context operator=(const Context&) const noexcept = delete;

	/**
	 * @brief create the renderer
	 *
	 * @param window the window
	 */
	void create(const Window* const window);

	/**
	 * @brief wait for queue to finish submitting
	 *
	 * @param queue queue to wait for
	 */
	void wait_device_queue(const VulkanDevice::VulkanQueueFamily queue) const;

	/**
	 * @brief add a function to the rendering queue
	 * 
	 * @param function the function
	*/
	void add_to_render_queue(std::function<void()>&& function);

	/**
	 * @brief take the recorded commands and draw everything
	 */
	void draw();

	/**
	 * @brief get the instance
	 *
	 * @return const lyra::VulkanInstance*
	*/
	[[nodiscard]] const VulkanInstance* instance() const noexcept { return &_instance; }
	/**
	 * @brief get the device
	 * 
	 * @return const lyra::VulkanDevice*
	*/
	[[nodiscard]] const VulkanDevice* device() const noexcept { return &_device; }
	/**
	 * @brief get the command pool
	 * 
	 * @return const lyra::VulkanCommandPool*
	*/
	[[nodiscard]] const VulkanCommandPool* commandPool() const noexcept { return &_commandPool; }
	/**
	 * @brief get the command buffers
	 *
	 * @return const std::vector<VulkanCommandBuffer>&
	*/
	[[nodiscard]] const std::vector<VulkanCommandBuffer>& commandBuffers() const noexcept { return _commandBuffers; }
	/**
	 * @brief get the swapchain
	 * 
	 * @return const lyra::VulkanSwapchain*
	*/
	[[nodiscard]] const VulkanSwapchain* swapchain() const noexcept { return &_swapchain; }
	/**
	 * @brief get the descriptor set layout
	 * 
	 * @return const lyra::VulkanDescriptorSetLayout*
	*/
	[[nodiscard]] const VulkanDescriptorSetLayout* descriptorSetLayout() const noexcept { return &_descriptorSetLayout; }
	/**
	 * @brief get the descriptor pool
	 * 
	 * @return const lyra::VulkanDescriptorPool*
	*/
	[[nodiscard]] const VulkanDescriptorPool* descriptorPool() const noexcept { return &_descriptorPool; }
	/**
	 * @brief get the queue with the draw calls
	 * 
	 * @return const CallQueue*
	*/
	[[nodiscard]] const CallQueue* renderQueue() const noexcept { return &_renderQueue; }
	/**
	 * @brief get the current frame count
	 * 
	 * @return const uint8
	*/
	[[nodiscard]] const uint8 currentFrame() const noexcept { return _currentFrame; }
	/**
	 * @brief get the image index
	 * 
	 * @return const uint32
	*/
	[[nodiscard]] const uint32 imageIndex() const noexcept { return _imageIndex; }

private:
	VulkanInstance _instance;
	VulkanDevice _device;
	VulkanCommandPool _commandPool;
	std::vector<VulkanCommandBuffer> _commandBuffers;
	VulkanSyncObjects _syncObjects;
	VulkanSwapchain _swapchain;
	VulkanDescriptorSetLayout _descriptorSetLayout;
	VulkanDescriptorPool _descriptorPool;

	CallQueue _renderQueue;

	uint8 _currentFrame = 0;
	uint32 _imageIndex;

	const Window* window;

	/**
	 * @brief recreate the swapchain and related stuff in case of some events
	 */
	void recreate_swapchain();

	/**
	 * @brief present all the recorded commands
	 */
	void present_device_queue();
	/**
	 * @brief submit the device presentation queue after command recording
	 *
	 * @param stageFlags pipeline shader stage flags
	 */
	void submit_device_queue(const VkPipelineStageFlags stageFlags) const;

	/**
	 * @brief update the frame count
	 */
	void update_frame_count() noexcept;
};

} // namespace Vulkan
