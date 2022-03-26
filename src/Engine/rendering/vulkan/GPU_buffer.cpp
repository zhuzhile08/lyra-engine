#include <rendering/vulkan/GPU_buffer.h>

namespace lyra {

VulkanGPUBuffer::VulkanGPUBuffer() {}

void VulkanGPUBuffer::destroy() noexcept {
	vmaDestroyBuffer(device->allocator(), _buffer, _memory);

	delete device;

	LOG_INFO("Succesfully destroyed Vulkan GPU buffer!")
}

void VulkanGPUBuffer::create(VulkanDevice device, VkDeviceSize size, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memUsage) {
	LOG_INFO("Creating Vulkan GPU memory buffer...")

	_size = size;

	this->device = new VulkanDevice(device);

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

	// memory allocation info
	VmaAllocationCreateInfo memAllocInfo = {
		0,
		memUsage,
		0,
		0,
		0,
		nullptr,
		nullptr,
		0
	};

	if (vmaCreateBuffer(device.allocator(), &bufferInfo, &memAllocInfo, &_buffer, &_memory, nullptr) != VK_SUCCESS) LOG_EXEPTION("Failed to create Vulkan GPU memory buffer!");

	LOG_INFO("Succesfully created Vulkan GPU buffer at ", GET_ADDRESS(this), "!", END_L)
}

void VulkanGPUBuffer::copy(const VulkanCommandPool commandPool, const VulkanGPUBuffer srcBuffer) {
	// create a temporary command buffer
	VulkanCommandBuffer commandBuffer;

	// create the command buffer
	commandBuffer.create(*device, commandPool);

	// start recording
	commandBuffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	// transfer the contents of the sorce to the destination buffer
	VkBufferCopy copyRegion{
		0,
		0,
		_size
	};

	vkCmdCopyBuffer(commandBuffer.get(), srcBuffer.buffer(), _buffer, 1, &copyRegion);

	commandBuffer.end();		// end recording

	VkSubmitInfo submitInfo{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,
		nullptr,
		0,
		nullptr,
		nullptr,
		1,
		commandBuffer.get_ptr(),
		0,
		nullptr
	};

	vkQueueSubmit(device->graphicsQueue().queue, 1, &submitInfo, VK_NULL_HANDLE);   // @todo maybe move these functions into the device?
	vkQueueWaitIdle(device->graphicsQueue().queue);		// wait for completion

	commandBuffer.destroy();		    // delete command buffer after copying

	LOG_INFO("Succesfully copied Vulkan GPU buffer at ", GET_ADDRESS(&srcBuffer), " to ", GET_ADDRESS(this), "!", END_L)
}

void VulkanGPUBuffer::copy_data(void* src) {
	void* data;
	if (vmaMapMemory(device->allocator(), _memory, &data) != VK_SUCCESS) LOG_EXEPTION("Failed to map buffer memory at ", GET_ADDRESS(_memory), "!")
		memcpy(data, src, (size_t)_size);
	vmaUnmapMemory(device->allocator(), _memory);
}

} // namespace lyra
