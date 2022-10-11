#include <rendering/vulkan/GPU_buffer.h>

#include <core/logger.h>
#include <core/util.h>

#include <rendering/vulkan/devices.h>
#include <rendering/vulkan/command_buffer.h>

#include <core/application.h>

namespace lyra {

namespace vulkan {

GPUBuffer::GPUBuffer(VkDeviceSize const size, VkBufferUsageFlags const bufferUsage, VmaMemoryUsage const memUsage) : m_size(size) {
	Logger::log_info("Creating Vulkan GPU memory buffer...");

	// log debugging messages
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

	// create buffer
	vassert(Application::renderSystem()->device()->createBuffer(bufferInfo, get_alloc_create_info(memUsage), m_buffer, m_memory),
		"create Vulkan GPU memory buffer");

	Logger::log_info("Successfully created Vulkan GPU buffer at ", get_address(this), "!", Logger::end_l());
}

GPUBuffer::~GPUBuffer() noexcept {
	// destroy the buffer
	vkDestroyBuffer(Application::renderSystem()->device()->device(), m_buffer, nullptr);

	Logger::log_info("Successfully destroyed Vulkan GPU buffer!");
}

void GPUBuffer::copy_data(const void* const src, const size_t copySize) {
	// map the memory
	void* data;
	lassert(Application::renderSystem()->device()->mapMemory(m_memory, &data) == VkResult::VK_SUCCESS, "Failed to map buffer memory at ", get_address(m_memory), "!");
	
	// copy the data into the mapped memory
#ifndef NDEBUG
	memcpy(data, src, (copySize == 0) ? static_cast<size_t>(m_size) : copySize);
#else
	// custom memcpy functionality, (probably) faster in release mode
	const char* s = (char*)src;
	char* d = (char*)data;

	for(size_t i = 0; i < (copySize == 0) ? static_cast<size_t>(m_size) : copySize; i++) d[i] = s[i];

	data = std::move(d);
#endif

	// unmap the memory
	Application::renderSystem()->device()->unmapMemory(m_memory);
}

void GPUBuffer::copy(const GPUBuffer* const srcBuffer) {
	// get a unused command buffer
	CommandBuffer cmdBuff(Application::renderSystem()->commandBuffers());
	// start recording
	cmdBuff.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	// transfer the contents of the sorce to the destination buffer
	VkBufferCopy copyRegion{
		0,
		0,
		m_size
	};
	cmdBuff.copyBuffer(srcBuffer->buffer(), m_buffer, copyRegion);

	// end recording
	cmdBuff.end();
	// submit the commands
	cmdBuff.submitQueue(Application::renderSystem()->device()->graphicsQueue().queue);
	// reset the command buffer
	cmdBuff.reset();

	Logger::log_debug("Successfully copied Vulkan GPU buffer at ", get_address(&srcBuffer), " to ", get_address(this), "!", Logger::end_l());
}

} // namespace vulkan

} // namespace lyra
