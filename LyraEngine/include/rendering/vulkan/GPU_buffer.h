/*************************
 * @file GPU_buffer.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief wrapper around the Vulkan buffers
 *
 * @date 2022-02-05
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <vulkan/vulkan.h>

#include <core/decl.h>
#include <rendering/vulkan/GPU_memory.h>

namespace lyra {

namespace vulkan {

/**
 * @brief wrapper around the Vulkan buffer
 */
class GPUBuffer : private GPUMemory {
public:
	GPUBuffer() { }
	/**
	 * @brief create the buffer
	 *
	 * @param size buffer size
	 * @param bufferUsage way to use the buffer
	 * @param memUsage way to use the memory
	 */
	GPUBuffer(const VkDeviceSize size, const VkBufferUsageFlags bufferUsage, const VmaMemoryUsage memUsage);

	/**
	 * @brief destructor of the buffer
	 */
	virtual ~GPUBuffer() noexcept;
	/**
	 * @brief destroy the buffer
	 */
	void destroy() noexcept {
		this->~GPUBuffer();
	}

	GPUBuffer operator=(const GPUBuffer&) const noexcept = delete;

	/**
	 * @brief copy a buffer to another
	 *
	 * @param device device
	 * @param commandBuffer command buffers
	 * @param srcBuffer the buffer to copy
	 * @param size the size of the buffer
	 */
	void copy(const GPUBuffer* const srcBuffer);

	/**
	 * @brief map GPU memory to normal memory, copy some stuff in there and unmap it
	 *
	 * @param src data to copy into the buffer
	 * @param copySize size of the data to copy, default is the size of the buffer memory
	 */
	void copy_data(const void* const src, const size_t copySize = 0);

	/**
	 * @brief get the information in a buffer for descriptor sets
	 * 
	 * @return const VkDescriptorBufferInfo
	*/
	NODISCARD const VkDescriptorBufferInfo get_descriptor_buffer_info() const noexcept {
		return {
			m_buffer, 0, m_size
		};
	}
	/**
	 * @brief return a memory barrier for this buffer
	 *
	 * @param srcAccessMask the original accessability for the buffer
	 * @param dstAccessMask the new accessability to transition to
	 * @param srcQueueFamily the original queue family of the buffer
	 * @param dstQueueFamily the queue family to transfer ownership to
	 *
	 * @return const VkBufferMemoryBarrier
	*/
	NODISCARD const VkBufferMemoryBarrier get_buffer_memory_barrier(
		const VkAccessFlags srcAccessMask, 
		const VkAccessFlags dstAccessMask,
		const uint32 srcQueueFamily = VK_QUEUE_FAMILY_IGNORED,
		const uint32 dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
	) const noexcept {
		return {
			VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
			nullptr,
			srcAccessMask,
			dstAccessMask,
			srcQueueFamily,
			dstQueueFamily,
			m_buffer,
			0,
			m_size
		};
	}

	/**
	 * @brief get the buffer
	 * 
	 * @return const VkBuffer&
	*/
	NODISCARD const VkBuffer& buffer() const noexcept { return m_buffer; }
	/**
	 * @brief get the memory
	 * 
	 * @return const VmaAllocation&
	*/
	NODISCARD const VmaAllocation& memory() const noexcept { return m_memory; };
	/**
	 * @brief get the size of the buffer
	 * 
	 * @return const VkDeviceSize&
	*/
	NODISCARD const VkDeviceSize& size() const noexcept { return m_size; };

private:
	VkBuffer m_buffer = VK_NULL_HANDLE;
	VkDeviceSize m_size = 0;
};

} // namespace vulkan

} // namespace lyra
