#include <rendering/vulkan/command_buffer.h>

#include <core/settings.h>

#include <rendering/vulkan/devices.h>

#include <core/application.h>

namespace lyra {

namespace vulkan {

CommandPool::CommandPool() {
	VkCommandPoolCreateInfo createInfo{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		nullptr,
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		Application::renderSystem.device.graphicsQueue().familyIndex
	};

	vassert(vkCreateCommandPool(Application::renderSystem.device.device(), &createInfo, nullptr, &m_commandPool), "create Vulkan command pool");
}

// command pool
CommandPool::~CommandPool() noexcept {
	vkDestroyCommandPool(Application::renderSystem.device.device(), m_commandPool, nullptr);
}

void CommandPool::reset() {
	vassert(Application::renderSystem.device.resetCommandPool(m_commandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT), "reset command pool");
}

// command buffer
CommandBuffer::CommandBuffer(const Usage& usage, const VkCommandBufferLevel& level) : m_usage(usage), m_commandPool(Application::renderSystem.commandPools[static_cast<uint32>(usage)]) {
	// locate the memory
	VkCommandBufferAllocateInfo allocInfo{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		nullptr,
		m_commandPool.commandPool(),
		level,
		1
	};

	// create the command buffers
	vassert(vkAllocateCommandBuffers(Application::renderSystem.device.device(), &allocInfo, &m_commandBuffer), "create Vulkan command buffer");
}

CommandBuffer::~CommandBuffer() {
	Application::renderSystem.device.freeCommandBuffer(m_commandPool.commandPool(), m_commandBuffer);
}

void CommandBuffer::begin() const {
	// some info about the recording
	VkCommandBufferBeginInfo beginInfo{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		nullptr,
		static_cast<VkCommandBufferUsageFlags>(m_usage),
		nullptr
	};

	// start recording
	vassert(vkBeginCommandBuffer(m_commandBuffer, &beginInfo), "start recording Vulkan command buffer");
}

void CommandBuffer::reset(const VkCommandBufferResetFlags& flags) const {
	vassert(vkResetCommandBuffer(m_commandBuffer, flags), "reset command buffer"); // reset the command buffer
}

void CommandBuffer::submitQueue(const VkQueue& queue) const {
	// queue submission info
	VkSubmitInfo submitInfo = {
		VK_STRUCTURE_TYPE_SUBMIT_INFO,
		nullptr,
		0,
		nullptr,
		nullptr,
		1,
		&m_commandBuffer,
		0,
		nullptr
	};

	// submit the queue
	vassert(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE), "submit Vulkan queue");
	vassert(vkQueueWaitIdle(queue), "wait for device queue");
}

} // namespace vulkan

} // namespace lyra
