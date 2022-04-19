/*************************
 * @file GPU_buffer.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief wrapper around the Vulkan buffers
 *
 * @date 2022-02-05
 *
 * @copyright Copyright (c) 2022 Zhile Zhu
 *************************/

#pragma once

#include <core/defines.h>
#include <core/rendering/vulkan/devices.h>
#include <core/rendering/vulkan/command_buffer.h>
#include <core/rendering/vulkan/GPU_memory.h>
#include <core/logger.h>

#include <cstring>
#include <vector>

#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief wrapper around the Vulkan buffer
 */
class VulkanGPUBuffer : private VulkanGPUMemory {
public:
	VulkanGPUBuffer();

	/**
	 * @brief destructor of the buffer
	 */
	virtual ~VulkanGPUBuffer() noexcept;

	/**
	 * @brief destroy the buffer
	 */
	void destroy() noexcept;

	VulkanGPUBuffer operator=(const VulkanGPUBuffer&) = delete;

	/**
	 * @brief create the buffer
	 *
	 * @param device device
	 */
	void create(const VulkanDevice* device, VkDeviceSize size, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memUsage);

	/**
	 * @brief copy a buffer to another
	 *
	 * @param device device
	 * @param commandBuffer command buffers
	 * @param srcBuffer the buffer to copy
	 * @param size the size of the buffer
	 */
	void copy(const VulkanCommandPool* commandPool, const VulkanGPUBuffer* srcBuffer);

	/**
	 * @brief map GPU memory to normal memory, copy some stuff in there and unmap it
	 *
	 * @param src data to copy into the buffer
	 */
	void copy_data(const void* src);

	/**
	 * @brief get the information in a buffer for descriptor sets
	 * 
	 * @return const VkDescriptorBufferInfo
	*/
	[[nodiscard]] const VkDescriptorBufferInfo get_descriptor_buffer_info() const noexcept;
	/**
	 * @brief return a memory barrier for this buffer
	 *
	 * @param srcAccessMask the original accessability for the buffer
	 * @param dstAccessMask the new accessability to transition to
	 * @param srcQueueFamily the original queue family of the buffer
	 * @param dstQueueFamily the queue family to transfer ownership to
	 *
	 * @return const VkBufferMemoryBarrier*
	*/
	[[nodiscard]] const VkBufferMemoryBarrier* get_buffer_memory_barrier(
		const VkAccessFlags srcAccessMask, 
		const VkAccessFlags dstAccessMask,
		const uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED,
		const uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
	) const noexcept;

	/**
	 * @brief get the buffer
	 * 
	 * @return const VkBuffer 
	*/
	[[nodiscard]] const VkBuffer buffer() const noexcept { return _buffer; }
	/**
	 * @brief get the memory
	 * 
	 * @return const VmaAllocation
	*/
	[[nodiscard]] const VmaAllocation memory() const noexcept { return _memory; };
	/**
	 * @brief get the size of the buffer
	 * 
	 * @return const VkDeviceSize
	*/
	[[nodiscard]] const VkDeviceSize size() const noexcept { return _size; };

private:
	VkBuffer _buffer = VK_NULL_HANDLE;
	VkDeviceSize _size = 0;

protected:
	const VulkanDevice* device;
};

} // namespace lyra
