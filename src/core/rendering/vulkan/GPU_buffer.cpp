#include <core/rendering/vulkan/GPU_buffer.h>

namespace lyra {

VulkanGPUBuffer::VulkanGPUBuffer() {}

VulkanGPUBuffer::~VulkanGPUBuffer() noexcept {
	vmaDestroyBuffer(device->allocator(), _buffer, _memory);

	LOG_INFO("Succesfully destroyed Vulkan GPU buffer!");
}

void VulkanGPUBuffer::destroy() noexcept {
	this->~VulkanGPUBuffer();
}

void VulkanGPUBuffer::create(const VulkanDevice* const device, VkDeviceSize const size, VkBufferUsageFlags const bufferUsage, VmaMemoryUsage const memUsage) {
	LOG_INFO("Creating Vulkan GPU memory buffer...");

	_size = size;

	this->device = device;

	LOG_DEBUG(TAB, "Size: ", size, " bytes (", size / 1000, " kilobytes)");
	LOG_DEBUG(TAB, "Buffer usage flag: ", bufferUsage);
	LOG_DEBUG(TAB, "Memory usage flag: ", memUsage);

	// buffer creation info
	VkBufferCreateInfo bufferInfo{
		VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		nullptr,
		0,
		size,
		bufferUsage,
		VK_SHARING_MODE_EXCLUSIVE,
		0,
		0
	};

	if (vmaCreateBuffer(device->allocator(), &bufferInfo, &get_alloc_create_info(memUsage), &_buffer, &_memory, nullptr) != VK_SUCCESS) LOG_EXEPTION("Failed to create Vulkan GPU memory buffer!");

	LOG_INFO("Succesfully created Vulkan GPU buffer at ", get_address(this), "!", END_L);
}

void VulkanGPUBuffer::copy(const VulkanCommandPool* const commandPool, const VulkanGPUBuffer* const srcBuffer) {
	// create a temporary command buffer
	VulkanCommandBuffer commandBuffer;
	commandBuffer.create(device, commandPool);
	// start recording
	commandBuffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	// transfer the contents of the sorce to the destination buffer
	VkBufferCopy copyRegion{
		0,
		0,
		_size
	};

	vkCmdCopyBuffer(commandBuffer.get(), srcBuffer->buffer(), _buffer, 1, &copyRegion);

	commandBuffer.end();		// end recording

	// submit the commands
	commandBuffer.submit_queue(device->graphicsQueue().queue);
	commandBuffer.wait_queue(device->graphicsQueue().queue);

	LOG_INFO("Succesfully copied Vulkan GPU buffer at ", get_address(&srcBuffer), " to ", get_address(this), "!", END_L);
}

void VulkanGPUBuffer::copy_data(const void* const src, const size_t copySize) {
	void* data;
	if (vmaMapMemory(device->allocator(), _memory, &data) != VK_SUCCESS) LOG_EXEPTION("Failed to map buffer memory at ", get_address(_memory), "!");
		memcpy(data, src, (copySize == 0) ? static_cast<size_t>(_size) : copySize);
	vmaUnmapMemory(device->allocator(), _memory);
}

const VkBufferMemoryBarrier VulkanGPUBuffer::get_buffer_memory_barrier(
	const VkAccessFlags srcAccessMask,
	const VkAccessFlags dstAccessMask,
	const uint32_t srcQueueFamily,
	const uint32_t dstQueueFamily
) const noexcept {
	return {
		VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
		nullptr,
		srcAccessMask,
		dstAccessMask,
		srcQueueFamily,
		dstQueueFamily,
		_buffer,
		0,
		_size
	};
}

const VkDescriptorBufferInfo VulkanGPUBuffer::get_descriptor_buffer_info() const noexcept {
	return {
		_buffer, 0, _size
	};
}

} // namespace lyra
