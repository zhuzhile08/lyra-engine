/*************************
 * @file command_buffer.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief a wrapper around the Vulkan command buffers
 *
 * @date 2022-02-05
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <core/defines.h>
#include <graphics/vulkan/devices.h>
#include <graphics/vulkan/command_buffer.h>
#include <core/logger.h>

#include <SDL.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief command pool
 */
struct VulkanCommandPool {
public:

	VulkanCommandPool();

	/**
	 * @brief destroy the VulkanCommandBuffer object
	 * @brief it works just like the destructor, but it can be called anywhere
	 */
	void destroy() noexcept;

	/**
	 * @brief create a Vulkan command pool to allocate the command buffers
	 *
	 * @param device device
	 */
	void create(const VulkanDevice* device);

	/**
	 * @brief get the command pool
	 *
	 * @return const VkCommandPool
	 */
	const VkCommandPool get() const noexcept { return commandPool; }
	/**
	 * @brief get the command pool as a pointer
	 *
	 * @return VkCommandPool
	 */
	const VkCommandPool* get_ptr() const noexcept { return &commandPool; }

private:
	VkCommandPool commandPool;

	const VulkanDevice* device;
};

/**
 * @brief command buffer
 */
struct VulkanCommandBuffer {
public:

	VulkanCommandBuffer();

	/**
	 * @brief destroy the VulkanCommandBuffer object
	 * @brief it works just like the destructor, but it can be called anywhere
	 */
	void destroy() noexcept;

	/**
	 * @brief create the Vulkan command buffers
	 *
	 * @param device device
	 * @param commandPool command pool
	 * @param level level of the command buffer
	 */
	void create(const VulkanDevice* device, const VulkanCommandPool* commandPool, const VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	/**
	 * @brief begin recording a commandBuffer
	 *
	 * @param usage what the recording will be used for
	 */
	void begin(const VkCommandBufferUsageFlags usage = 0) const;
	/**
	 * @brief end recording a commandBuffer
	 */
	void end() const;
	/**
	 * reset the command buffer after everything has been recorded
	 *
	 * @flags additional flags
	 */
	void reset(VkCommandBufferResetFlags flags = 0) const;

	/**
	 * @brief submit a Vulkan queue after command queue recording. DO NOT confuse with the submit function in the renderer class. This is ONLY for small, local submits for one time commands
	 *
	 * @param queue the queue to submit
	 */
	void submit_queue(const VkQueue queue) const;

	/**
	 * @brief wait for the queue to finish
	 *
	 * @param queue the queue to wait for
	*/
	void wait_queue(const VkQueue queue) const;


	/**
	 * @brief get the command buffer
	 *
	 * @return const VkCommandBuffer
	 */
	const VkCommandBuffer get() const noexcept { return commandBuffer; }

	/**
	 * @brief get the command buffer as a pointer
	 *
	 * @return const VkCommandBuffer*
	 */
	const VkCommandBuffer* get_ptr() const noexcept { return &commandBuffer; }

private:
	VkCommandBuffer commandBuffer;

	const VulkanDevice* device;
	const VulkanCommandPool* commandPool;
};

} // namespace lyra
