#include <rendering/vulkan/command_buffer.h>

#include <core/settings.h>

#include <rendering/vulkan/devices.h>

#include <core/application.h>

namespace lyra {

namespace vulkan {

CommandPool::CommandPool() {
	Logger::log_info("Creating Vulkan command pool...");

	VkCommandPoolCreateInfo createInfo{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		nullptr,
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		Application::renderSystem()->device()->graphicsQueue().familyIndex
	};

	vassert(vkCreateCommandPool(Application::renderSystem()->device()->device(), &createInfo, nullptr, &m_commandPool), "create Vulkan command pool");

	Logger::log_info("Successfully created Vulkan command pool at ", get_address(this), "!", Logger::end_l());
}

// command pool
CommandPool::~CommandPool() noexcept {
	vkDestroyCommandPool(Application::renderSystem()->device()->device(), m_commandPool, nullptr);

	Logger::log_info("Successfully destroyed Vulkan command pool!");
}

void CommandPool::reset() {
	vassert(Application::renderSystem()->device()->resetCommandPool(m_commandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT), "reset command pool");
}

// command buffer
CommandBufferManager::VulkanCommandBuffer::VulkanCommandBuffer(const CommandPool* const commandPool, const VkCommandBufferLevel& level) :
	commandPool(commandPool)
{
	Logger::log_debug(Logger::tab(), "Creating Vulkan command buffer...");

	// locate the memory
	VkCommandBufferAllocateInfo allocInfo{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		nullptr,
		commandPool->commandPool(),
		level,
		1
	};

	// create the command buffers
	vassert(vkAllocateCommandBuffers(Application::renderSystem()->device()->device(), &allocInfo, &commandBuffer), "create Vulkan command buffer");

	Logger::log_debug(Logger::tab(), "Successfully created Vulkan command buffer at ", get_address(this), "!", Logger::end_l());
}

CommandBufferManager::VulkanCommandBuffer::~VulkanCommandBuffer() noexcept {
	Application::renderSystem()->device()->freeCommandBuffers(commandPool->commandPool(), 1, commandBuffer);

	Logger::log_info("Successfully destroyed a Vulkan command buffer!");
}

// manager
CommandBufferManager::CommandBufferManager(const VkCommandBufferLevel& level) : m_commandPool() {
	Logger::log_info("Creating command buffer manager...");

	m_commandBufferData.reserve(Settings::Memory::maxCommandBuffers);

	for (uint32 i = 0; i < Settings::Memory::maxCommandBuffers; i++) {
		m_commandBufferData.emplace_back(&m_commandPool, level);
		m_commandBuffers.emplace(i, CommandBufferUsage::COMMAND_BUFFER_UNUSED);
	}

	Logger::log_info("Successfully created command buffer manager at ", get_address(this), "!", Logger::end_l());
}

CommandBufferManager::~CommandBufferManager() noexcept {
	m_commandBufferData.clear();

	Logger::log_info("Successfully destroyed a command buffer manager!");
}

void CommandBuffer::begin(const VkCommandBufferUsageFlags& usage) {
	commandBufferManager->m_commandBuffers.find(m_index)->second = CommandBufferManager::CommandBufferUsage::COMMAND_BUFFER_USED; // set that command buffer as in use

	// some info about the recording
	VkCommandBufferBeginInfo beginInfo{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		nullptr,
		usage,
		nullptr
	};

	// start recording
	vassert(vkBeginCommandBuffer(*m_commandBuffer, &beginInfo), "start recording Vulkan command buffer");
}

void CommandBuffer::reset(const VkCommandBufferResetFlags& flags) {
	commandBufferManager->m_commandBuffers.find(m_index)->second = CommandBufferManager::CommandBufferUsage::COMMAND_BUFFER_UNUSED; // set that command buffer as unused

	vassert(vkResetCommandBuffer(*m_commandBuffer, flags), "reset command buffer"); // reset the command buffer
}

void CommandBuffer::submitQueue(const VkQueue& queue) {
	// queue submission info
	VkSubmitInfo submitInfo = {
		VK_STRUCTURE_TYPE_SUBMIT_INFO,
		nullptr,
		0,
		nullptr,
		nullptr,
		1,
		m_commandBuffer,
		0,
		nullptr
	};

	// submit the queue
	vassert(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE), "submit Vulkan queue");
	vassert(vkQueueWaitIdle(queue), "wait for device queue");
}

} // namespace vulkan

} // namespace lyra
