/*************************
 * @file GPUBuffer.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief wrapper around the Vulkan buffers
 *
 * @date 2022-02-05
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <Lyra/Lyra.h>

#include <vulkan/vulkan.h>

#include <Graphics/VulkanImpl/GPUMemory.h>

namespace lyra {

namespace vulkan {

/**
 * @brief wrapper around the Vulkan buffer
 */
class GPUBuffer : public GPUMemory {
public:
	constexpr GPUBuffer() = default;
	/**
	 * @brief create the buffer
	 *
	 * @param size buffer size
	 * @param bufferUsage way to use the buffer
	 * @param memUsage way to use the memory
	 */
	GPUBuffer(const VkDeviceSize& size, const VkBufferUsageFlags& bufferUsage, const VmaMemoryUsage& memUsage);
	DEFINE_DEFAULT_MOVE(GPUBuffer);

	/**
	 * @brief copy a buffer to another
	 *
	 * @param srcBuffer buffer to copy the data from
	 */
	void copy(const GPUBuffer& srcBuffer);

	/**
	 * @brief map GPU memory to normal memory, copy some stuff in there and unmap it
	 *
	 * @param src data to copy into the buffer
	 * @param copySize size of the data to copy, default is the size of the buffer memory
	 */
	void copy_data(const void* src, const size_t& copySize = 0);
	/**
	 * @brief copy data from an array into the buffer element by element
	 * 
	 * @param src source array
	 * @param arraySize size of the array
	 * @param elementSize size a single array element
	 */
	void copy_data(const void** src, const uint32& arraySize, const size_t& elementSize = 0);

	/**
	 * @brief get the information in a buffer for descriptor sets
	 * 
	 * @return constexpr VkDescriptorBufferInfo
	*/
	NODISCARD constexpr VkDescriptorBufferInfo get_descriptor_buffer_info() const noexcept {
		return {
			m_buffer, 
			0, 
			m_size
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
	 * @return constexpr VkBufferMemoryBarrier
	*/
	NODISCARD constexpr VkBufferMemoryBarrier get_buffer_memory_barrier(
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
	 * @return constexpr lyra::vulkan::vk::Buffer&
	*/
	NODISCARD constexpr const vk::Buffer& buffer() const noexcept { return m_buffer; }
	/**
	 * @brief get the size of the buffer
	 * 
	 * @return constexpr VkDeviceSize
	*/
	NODISCARD constexpr VkDeviceSize size() const noexcept { return m_size; };

private:
	vk::Buffer m_buffer;
	VkDeviceSize m_size = 0;
};

} // namespace vulkan

} // namespace lyra
