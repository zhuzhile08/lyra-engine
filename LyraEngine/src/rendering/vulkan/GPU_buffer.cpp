#include <rendering/vulkan/GPU_buffer.h>

#include <rendering/vulkan/devices.h>
#include <rendering/vulkan/command_buffer.h>

#include <core/application.h>

namespace lyra {

namespace vulkan {

constexpr GPUBuffer::GPUBuffer(const VkDeviceSize& size, const VkBufferUsageFlags& bufferUsage, const VmaMemoryUsage& memUsage) : m_size(size) {
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
	vassert(Application::renderSystem.device.createBuffer(bufferInfo, get_alloc_create_info(memUsage), m_buffer, m_memory),
		"create Vulkan GPU memory buffer");
}

void GPUBuffer::copy_data(const void* src, const size_t& copySize) {
	// map the memory
	void* data;
	lassert(Application::renderSystem.device.mapMemory(m_memory, &data) == VkResult::VK_SUCCESS, "Failed to map buffer memory at ", get_address(m_memory), "!");
	
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
	Application::renderSystem.device.unmapMemory(m_memory);
}

void GPUBuffer::copy_data(const void** src, const uint32& arraySize, const size_t& elementSize) {
	// map the memory
	char* data;
	lassert(Application::renderSystem.device.mapMemory(m_memory, (void**)&data) == VkResult::VK_SUCCESS, "Failed to map buffer memory at ", get_address(m_memory), "!");

	// loop through the array
	for (uint32 i = 0; i < arraySize; i++) {
		// copy the data into the mapped memory
		memcpy(static_cast<void*>(data + elementSize * i), src[i], elementSize);
	}

	// unmap the memory
	Application::renderSystem.device.unmapMemory(m_memory);
}

void GPUBuffer::copy(const GPUBuffer* const srcBuffer) {
	// get a unused command buffer
	CommandBuffer cmdBuff(Application::renderSystem.commandBuffers);
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
	cmdBuff.submitQueue(Application::renderSystem.device.graphicsQueue().queue);
	// reset the command buffer
	cmdBuff.reset();
}

} // namespace vulkan

} // namespace lyra
