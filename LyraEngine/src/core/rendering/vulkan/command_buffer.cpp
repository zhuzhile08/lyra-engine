#include <core/rendering/vulkan/command_buffer.h>

#include <core/logger.h>

#include <core/application.h>
#include <core/rendering/vulkan/devices.h>

namespace lyra {

VulkanCommandPool::VulkanCommandPool() {
	Logger::log_info("Creating Vulkan command pool...");

	VkCommandPoolCreateInfo createInfo{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		nullptr,
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		Application::renderSystem()->device()->graphicsQueue().familyIndex
	};

	lassert(vkCreateCommandPool(Application::renderSystem()->device()->device(), &createInfo, nullptr, &_commandPool) == VK_SUCCESS, "Failed to create Vulkan command pool");

	Logger::log_info("Successfully created Vulkan command pool at ", get_address(this), "!", Logger::end_l());
}

// command pool
VulkanCommandPool::~VulkanCommandPool() noexcept {
	vkDestroyCommandPool(Application::renderSystem()->device()->device(), _commandPool, nullptr);

	Logger::log_info("Successfully destroyed Vulkan command pool!");
}

void VulkanCommandPool::reset() {
	lassert(vkResetCommandPool(Application::renderSystem()->device()->device(), _commandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT), "Failed to reset command pool!");
}

// command buffer
CommandBufferManager::VulkanCommandBuffer::VulkanCommandBuffer(const VulkanCommandPool* const commandPool, const VkCommandBufferLevel level) : 
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
	lassert(vkAllocateCommandBuffers(Application::renderSystem()->device()->device(), &allocInfo, &commandBuffer) == VK_SUCCESS, "Failed to create Vulkan command buffer!");

	Logger::log_debug(Logger::tab(), "Successfully created Vulkan command buffer at ", get_address(this), "!", Logger::end_l());
}

CommandBufferManager::VulkanCommandBuffer::~VulkanCommandBuffer() noexcept {
	vkFreeCommandBuffers(Application::renderSystem()->device()->device(), commandPool->commandPool(), 1, &commandBuffer);

	Logger::log_info("Successfully destroyed a Vulkan command buffer!");
}

// manager
CommandBufferManager::CommandBufferManager(const VkCommandBufferLevel level) : _commandPool() {
	Logger::log_info("Creating command buffer manager...");

	_commandBufferData.reserve(Settings::Memory::maxCommandBuffers);

	for (uint32 i = 0; i < Settings::Memory::maxCommandBuffers; i++) {
		_commandBufferData.emplace_back(&_commandPool, level);
		_commandBuffers.emplace(i, CommandBufferUsage::COMMAND_BUFFER_UNUSED);
	}

	Logger::log_info("Successfully created command buffer manager at ", get_address(this), "!", Logger::end_l());
}

CommandBufferManager::~CommandBufferManager() noexcept {
	_commandBufferData.clear();

	Logger::log_info("Successfully destroyed a command buffer manager!");
}

void CommandBufferManager::begin(const CommandBuffer cmdBuffer, const VkCommandBufferUsageFlags usage) {
	_commandBuffers.find(cmdBuffer)->second = CommandBufferUsage::COMMAND_BUFFER_USED; // set that command buffer as in use

	// some info about the recording
	VkCommandBufferBeginInfo beginInfo{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		nullptr,
		usage,
		nullptr
	};

	// start recording
	lassert(vkBeginCommandBuffer(_commandBufferData.at(cmdBuffer).commandBuffer, &beginInfo) == VK_SUCCESS, "Failed to start recording Vulkan command buffer!");
}

void CommandBufferManager::end(const CommandBuffer cmdBuffer) const {
	lassert(vkEndCommandBuffer(_commandBufferData.at(cmdBuffer).commandBuffer) == VK_SUCCESS, "Failed to stop recording command buffer!");
}

void CommandBufferManager::reset(const CommandBuffer cmdBuffer, const VkCommandBufferResetFlags flags) {
	_commandBuffers.find(cmdBuffer)->second = CommandBufferUsage::COMMAND_BUFFER_UNUSED; // set that command buffer as unused

	lassert(vkResetCommandBuffer(_commandBufferData.at(cmdBuffer).commandBuffer, flags) == VK_SUCCESS, "Failed to reset command buffer!"); // reset the command buffer
}

void CommandBufferManager::submit_queue(const CommandBuffer cmdBuffer, const VkQueue queue) const {
	// queue submission info
	VkSubmitInfo submitInfo = {
		VK_STRUCTURE_TYPE_SUBMIT_INFO,
		nullptr,
		0,
		nullptr,
		nullptr,
		1,
		&_commandBufferData.at(cmdBuffer).commandBuffer,
		0,
		nullptr
	};

	// submit the queue
	lassert(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE) == VK_SUCCESS, "Failed to submit Vulkan queue!");
}

void CommandBufferManager::wait_queue(const VkQueue queue) const {
	lassert(vkQueueWaitIdle(queue) == VK_SUCCESS, "Failed to wait for device queue!");
}

} // namespace lyra
