#include <core/rendering/vulkan/GPU_buffer.h>

#include <core/logger.h>

#include <core/application.h>
#include <core/rendering/vulkan/devices.h>
#include <core/rendering/vulkan/command_buffer.h>

namespace lyra {

VulkanGPUBuffer::VulkanGPUBuffer(VkDeviceSize const size, VkBufferUsageFlags const bufferUsage, VmaMemoryUsage const memUsage) {
	Logger::log_info("Creating Vulkan GPU memory buffer...");

	_size = size;

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

	lassert(vmaCreateBuffer(Application::renderSystem()->device()->allocator(), &bufferInfo, &get_alloc_create_info(memUsage), &_buffer, &_memory, nullptr) == VK_SUCCESS,
		"Failed to create Vulkan GPU memory buffer!");

	Logger::log_info("Successfully created Vulkan GPU buffer at ", get_address(this), "!", Logger::end_l());
}

VulkanGPUBuffer::~VulkanGPUBuffer() noexcept {
	vkDestroyBuffer(Application::renderSystem()->device()->device(), _buffer, nullptr);

	Logger::log_info("Successfully destroyed Vulkan GPU buffer!");
}

void VulkanGPUBuffer::copy_data(const void* const src, const size_t copySize) {
	void* data;
	lassert(vmaMapMemory(Application::renderSystem()->device()->allocator(), _memory, &data) == VK_SUCCESS, "Failed to map buffer memory at ", get_address(_memory), "!");

	/**  
	const char* s = (char*)src;
	char* d = (char*)data;

	for(size_t i = 0; i < (copySize == 0) ? static_cast<size_t>(_size) : copySize; i++) d[i] = s[i];

	data = std::move(d);
	*/

	memcpy(data, src, (copySize == 0) ? static_cast<size_t>(_size) : copySize);

	vmaUnmapMemory(Application::renderSystem()->device()->allocator(), _memory);
}

void VulkanGPUBuffer::copy(const VulkanGPUBuffer* const srcBuffer) {
	// get a unused command buffer
	CommandBuffer cmdBuff = Application::renderSystem()->commandBuffers()->get_unused();
	// start recording
	Application::renderSystem()->commandBuffers()->begin(cmdBuff, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	// transfer the contents of the sorce to the destination buffer
	VkBufferCopy copyRegion{
		0,
		0,
		_size
	};

	vkCmdCopyBuffer(Application::renderSystem()->commandBuffers()->commandBuffer(cmdBuff)->commandBuffer, srcBuffer->buffer(), _buffer, 1, &copyRegion);

	Application::renderSystem()->commandBuffers()->end(cmdBuff);		// end recording

	// submit the commands
	Application::renderSystem()->commandBuffers()->submit_queue(cmdBuff, Application::renderSystem()->device()->graphicsQueue().queue);
	Application::renderSystem()->commandBuffers()->wait_queue(Application::renderSystem()->device()->graphicsQueue().queue);
	// reset the command buffer
	Application::renderSystem()->commandBuffers()->reset(cmdBuff);

	Logger::log_debug("Successfully copied Vulkan GPU buffer at ", get_address(&srcBuffer), " to ", get_address(this), "!", Logger::end_l());
}

} // namespace lyra
