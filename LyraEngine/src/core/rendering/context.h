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

#include <core/core.h>

#include <vector>

#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief a vulkan context containing everything needed for an application
 */
class Context {
public:
	Context() { }

	/**
	 * @brief destructor of the context
	 */
	~Context() noexcept;

	/**
	 * @brief destroy the context
	 */
	void destroy() noexcept {
		this->~Context();
	}

	Context(const Context&) noexcept = delete;
	Context operator=(const Context&) const noexcept = delete;

	/**
	 * @brief create the renderer
	 *
	 * @param window the window
	 */
	void create(Window* const window);

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
	void add_to_render_queue(std::function<void()>&& function) {
		_renderQueue->add(std::move(function));
	}
	/**
	 * @brief add a function to the update queue
	 *
	 * @param function the function
	*/
	void add_to_update_queue(std::function<void()>&& function) {
		_renderQueue->add(std::move(function));
	}
	/**
	 * @brief add a function to the recreate queue
	 *
	 * @param function the function
	*/
	void add_to_recreate_queue(std::function<void()>&& function) {
		_recreateQueue->add(std::move(function));
	}

	/**
	 * @brief take the recorded commands and draw everything
	 */
	void draw();

	/**
	 * @brief update all the members
	*/
	void update() const { _updateQueue->flush(); }

	/**
	 * @brief get the device
	 * 
	 * @return const lyra::VulkanDevice* const
	*/
	[[nodiscard]] const VulkanDevice* const device() const noexcept { return _device; }
	/**
	 * @brief get the command pool
	 * 
	 * @return const lyra::VulkanCommandPool* const
	*/
	[[nodiscard]] const VulkanCommandPool* const commandPool() const noexcept { return _commandPool; }
	/**
	 * @brief get the command buffers
	 *
	 * @return constCommandBufferManager* const
	*/
	[[nodiscard]] CommandBufferManager* const commandBuffers() noexcept { return _commandBuffers; }
	/**
	 * @brief get the vulkanWindow
	 * 
	 * @return const lyra::VulkanWindow* const
	*/
	[[nodiscard]] const VulkanWindow* const vulkanWindow() const noexcept { return _vulkanWindow; }
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
	/**
	 * @brief get the image index
	 *
	 * @return const lyra::CommandBuffer
	*/
	[[nodiscard]] const CommandBuffer currentCommandBuffer() const noexcept { return _currentCommandBuffer; }

private:
	VulkanDevice* _device;
	VulkanCommandPool* _commandPool;
	CommandBufferManager* _commandBuffers;
	VulkanWindow* _vulkanWindow;

	CallQueue* _updateQueue;
	CallQueue* _renderQueue;
	CallQueue* _recreateQueue;

	uint8 _currentFrame = 0;
	uint32 _imageIndex;
	
	CommandBuffer _currentCommandBuffer;

	Window* window;

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
	void update_frame_count() noexcept {
		_currentFrame = (_currentFrame + 1) % Settings::Rendering::maxFramesInFlight;
	}
};

} // namespace Vulkan
