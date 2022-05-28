/*************************
 * @file command_buffer.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief a wrapper around the Vulkan command buffers
 *
 * @date 2022-02-05
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <core/defines.h>
#include <core/rendering/vulkan/devices.h>
#include <core/logger.h>

#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief command pool
 */
struct VulkanCommandPool {
public:

	VulkanCommandPool();

	/**
	* @brief destructor of the command pool
	**/
	virtual ~VulkanCommandPool() noexcept {
		vkDestroyCommandPool(device->device(), commandPool, nullptr);

		Logger::log_info("Succesfully destroyed Vulkan command pool!");
	}

	/**
	 * @brief destroy the command pool
	 */
	void destroy() noexcept {
		this->~VulkanCommandPool();
	}

	VulkanCommandPool operator=(const VulkanCommandPool&) const noexcept = delete;

	/**
	 * @brief create a Vulkan command pool to allocate the command buffers
	 *
	 * @param device device
	 */
	void create(const VulkanDevice* const device);

	/**
	 * @brief get the command pool
	 *
	 * @return const VkCommandPool&
	 */
	[[nodiscard]] const VkCommandPool& get() const noexcept { return commandPool; }
	/**
	 * @brief get the command pool as a pointer
	 *
	 * @return VkCommandPool
	 */
	[[nodiscard]] const VkCommandPool* const get_ptr() const noexcept { return &commandPool; }

private:
	VkCommandPool commandPool = VK_NULL_HANDLE;

	const VulkanDevice* device;
};

/**
 * @brief command buffer
 */
struct VulkanCommandBuffer {
public:

	VulkanCommandBuffer();

	/**
	* @brief destructor of the command buffer
	**/
	virtual ~VulkanCommandBuffer() noexcept {
		vkFreeCommandBuffers(device->device(), commandPool->get(), 1, &commandBuffer);

		Logger::log_info("Succesfully destroyed Vulkan command buffer!");
	}

	/**
	 * @brief destroy the command buffer
	 */
	void destroy() noexcept {
		this->~VulkanCommandBuffer();
	}

	VulkanCommandBuffer operator=(const VulkanCommandBuffer&) const noexcept = delete;

	/**
	 * @brief create the Vulkan command buffers
	 *
	 * @param device device
	 * @param commandPool command pool
	 * @param level level of the command buffer
	 */
	void create(const VulkanDevice* const device, const VulkanCommandPool* const commandPool, const VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	/**
	 * @brief begin recording a commandBuffer
	 *
	 * @param usage what the recording will be used for
	 */
	void begin(const VkCommandBufferUsageFlags usage = 0) const {
		// some info about the recording
		VkCommandBufferBeginInfo beginInfo{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			nullptr,
			usage,
			nullptr
		};

		// start recording
		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) Logger::log_exception("Failed to start recording Vulkan command buffer!");
	}
	/**
	 * @brief end recording a commandBuffer
	 */
	void end() const {
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) Logger::log_exception("Failed to stop recording command buffer!");
	}
	/**
	 * reset the command buffer after everything has been recorded
	 *
	 * @flags additional flags
	 */
	void reset(const VkCommandBufferResetFlags flags = 0) const {
		if (vkResetCommandBuffer(commandBuffer, flags) != VK_SUCCESS) Logger::log_exception("Failed to reset command buffer!");

		Logger::log_debug(Logger::tab(), "Reset command buffer at: ", get_address(this));
	}

	/**
	 * @brief submit a Vulkan queue after command queue recording. DO NOT confuse with the submit function in the renderer class. This is ONLY for small, local submits for one time commands
	 *
	 * @param queue the queue to submit
	 */
	void submit_queue(const VkQueue queue) const {
		// queue submission info
		VkSubmitInfo submitInfo = {
			VK_STRUCTURE_TYPE_SUBMIT_INFO,
			nullptr,
			0,
			nullptr,
			nullptr,
			1,
			&commandBuffer,
			0,
			nullptr
		};

		// submit the queue
		if (vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) Logger::log_exception("Failed to submit Vulkan queue!");

		Logger::log_debug(Logger::tab(), "Submitted command buffer at: ", get_address(this));
	}

	/**
	 * @brief wait for the queue to finish
	 *
	 * @param queue the queue to wait for
	*/
	void wait_queue(const VkQueue queue) const {
		if (vkQueueWaitIdle(queue) != VK_SUCCESS) Logger::log_exception("Failed to wait for device queue!");
	}

	/**
	 * @brief setup a pipeline barrier
	 * 
	 * @param srcStageFlags the pipeline stage to wait for
	 * @param dstStageFlags the pipeline stage to skip to
	 * @param memory if there is, then the memory to transition
	 * @param buffer if there is, then the buffer to transition
	 * @param image if there is, then the image to transition
	 * @param dependency I have no idea, just leave it zero
	*/
	void pipeline_barrier(
		const VkPipelineStageFlags srcStageFlags, 
		const VkPipelineStageFlags dstStageFlags,
		const VkMemoryBarrier* const memory = nullptr,
		const VkBufferMemoryBarrier* const buffer = nullptr,
		const VkImageMemoryBarrier* const image = nullptr,
		const VkDependencyFlags dependency = 0
	) const {
		vkCmdPipelineBarrier(
			commandBuffer,
			srcStageFlags,
			dstStageFlags,
			dependency,
			(memory == nullptr) ? 0 : 1,
			memory,
			(buffer == nullptr) ? 0 : 1,
			buffer,
			(image == nullptr) ? 0 : 1,
			image
		);

		Logger::log_debug(Logger::tab(), "Setup a pipeline barrier with the command buffer at: ", get_address(this));
	}

	/**
	 * @brief get the command buffer
	 *
	 * @return const VkCommandBuffer&
	 */
	[[nodiscard]] const VkCommandBuffer& get() const noexcept { return commandBuffer; }

	/**
	 * @brief get the command buffer as a pointer
	 *
	 * @return const VkCommandBuffer*
	 */
	[[nodiscard]] const VkCommandBuffer* const get_ptr() const noexcept { return &commandBuffer; }

private:
	VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

	const VulkanDevice* device;
	const VulkanCommandPool* commandPool;
};

} // namespace lyra
