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
#include <graphics/vulkan/devices.h>
#include <graphics/vulkan/command_buffer.h>
#include <graphics/vulkan/GPU_memory.h>
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
	 * @brief destroy the VulkanGPUBuffer object
	 */
	void destroy() noexcept;

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
	 * @return true
	 */
	void copy(const VulkanCommandPool* commandPool, const VulkanGPUBuffer srcBuffer);

	/**
	 * @brief map GPU memory to normal memory, copy some stuff in there and unmap it
	 *
	 * @param src data to copy into the buffer
	 */
	void copy_data(void* src);

	/**
	 * @brief get the information in a buffer for descriptor sets
	 * 
	 * @return const VkDescriptorBufferInfo
	*/
	const VkDescriptorBufferInfo get_descriptor_buffer_info() const noexcept { return {_buffer, 0, _size}; }
	/**
	 * @brief get the buffer
	 * 
	 * @return const VkBuffer 
	*/
	const VkBuffer buffer() const noexcept { return _buffer; }
	/**
	 * @brief get the memory
	 * 
	 * @return const VmaAllocation
	*/
	const VmaAllocation memory() const noexcept { return _memory; };
	/**
	 * @brief get the size of the buffer
	 * 
	 * @return  const VkDeviceSize
	*/
	const VkDeviceSize size() const noexcept { return _size; };

private:
	VkBuffer _buffer;
	VkDeviceSize _size;

protected:
	const VulkanDevice* device;
};

} // namespace lyra
