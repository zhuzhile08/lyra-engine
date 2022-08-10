/*************************
 * @file render_system.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief a rendering system around the Vulkan API with basic features
 *
 * @date 2022-02-04
 *
 * @copyright Copyright (c) 2022 Zhile Zhu
 *************************/

#pragma once

#include <core/decl.h>
#include <core/rendering/vulkan/devices.h>

#include <vector>
#include <memory>

#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief a vulkan context containing everything needed for an application
 */
class RenderSystem {
public:
	RenderSystem() { }
	/**
	 * @brief create the renderer
	 *
	 * @param window the window
	 */
	RenderSystem(Window* const window);

	/**
	 * @brief destructor of the context
	 */
	~RenderSystem() noexcept;

	/**
	 * @brief destroy the context
	 */
	void destroy() noexcept {
		this->~RenderSystem();
	}

	RenderSystem(const RenderSystem&) noexcept = delete;
	RenderSystem operator=(const RenderSystem&) const noexcept = delete;

	/**
	 * @brief wait for queue to finish submitting
	 *
	 * @param queue queue to wait for
	 */
	void wait_device_queue(const VulkanDevice::VulkanQueueFamily queue) const;

	/**
	 * @brief take the recorded commands and draw everything
	 */
	void draw();

	/**
	 * @brief update all the members
	*/
	void update() const;

	/**
	 * @brief get the device
	 * 
	 * @return const std::shared_ptr<const VulkanDevice>
	*/
	[[nodiscard]] const std::shared_ptr<const VulkanDevice> device() const noexcept { return _device; }
	/**
	 * @brief get the command pool
	 * 
	 * @return const std::shared_ptr<const VulkanCommandPool>
	*/
	[[nodiscard]] const std::shared_ptr<const VulkanCommandPool> commandPool() const noexcept { return _commandPool; }
	/**
	 * @brief get the command buffers
	 *
	 * @return const std::shared_ptr<CommandBufferManager>
	*/
	[[nodiscard]] const std::shared_ptr<CommandBufferManager> commandBuffers() noexcept { return _commandBuffers; }
	/**
	 * @brief get the vulkan window
	 *
	 * @return const lyra::VulkanWindow* const
	*/
	[[nodiscard]] const std::shared_ptr<const VulkanWindow> vulkanWindow() const noexcept { return _vulkanWindow; }
	/**
	 * @brief get the current active command buffer
	 *
	 * @return const VkCommandBuffer&
	*/
	[[nodiscard]] const VkCommandBuffer& activeCommandBuffer() noexcept;
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
	 * @return const lyra::CommandBuffer&
	*/
	[[nodiscard]] const CommandBuffer& currentCommandBuffer() const noexcept { return _currentCommandBuffer; }

private:
	std::shared_ptr<VulkanDevice> _device;
	std::shared_ptr<VulkanCommandPool> _commandPool;
	std::shared_ptr<CommandBufferManager> _commandBuffers;
	std::shared_ptr<VulkanWindow> _vulkanWindow;

	std::vector<Renderer*> _renderers;

	uint8 _currentFrame = 0;
	uint32 _imageIndex;
	
	CommandBuffer _currentCommandBuffer;

	Window* window;


	/**
	 * @brief add a renderer to the vector of renderers
	 *
	 * @param renderer renderer
	*/
	void add_renderer(Renderer* const renderer);

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

	friend class Renderer;
	friend class Camera;
};

} // namespace Vulkan
