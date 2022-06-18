#include <core/rendering/vulkan/GPU_buffer.h>

namespace lyra {

VulkanGPUBuffer::VulkanGPUBuffer() {}

void VulkanGPUBuffer::create(const VulkanDevice* const device, VkDeviceSize const size, VkBufferUsageFlags const bufferUsage, VmaMemoryUsage const memUsage) {
	Logger::log_info("Creating Vulkan GPU memory buffer...");

	_size = size;

	this->device = device;

	Logger::log_debug(Logger::tab(), "Size: ", size, " bytes (", size / 1000, " kilobytes)");
	Logger::log_debug(Logger::tab(), "Buffer usage flag: ", bufferUsage);
	Logger::log_debug(Logger::tab(), "Memory usage flag: ", memUsage);

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

	if (vmaCreateBuffer(device->allocator(), &bufferInfo, &get_alloc_create_info(Application::context()->device(), memUsage), &_buffer, &_memory, nullptr) != VK_SUCCESS) 
		Logger::log_exception("Failed to create Vulkan GPU memory buffer!");

	Logger::log_info("Successfully created Vulkan GPU buffer at ", get_address(this), "!", Logger::end_l());
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

	Logger::log_debug("Successfully copied Vulkan GPU buffer at ", get_address(&srcBuffer), " to ", get_address(this), "!", Logger::end_l());
}

} // namespace lyra
