#include <core/rendering/vulkan/command_buffer.h>

#include <core/logger.h>

#include <core/rendering/vulkan/devices.h>

namespace lyra {

// command pool
VulkanCommandPool::~VulkanCommandPool() noexcept {
	vkDestroyCommandPool(device->device(), _commandPool, nullptr);

	Logger::log_info("Successfully destroyed Vulkan command pool!");
}

void VulkanCommandPool::create(const VulkanDevice* const device) {
	Logger::log_info("Creating Vulkan command pool...");

	this->device = device;

	VkCommandPoolCreateInfo createInfo{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		nullptr,
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		device->graphicsQueue().familyIndex
	};

	lassert(vkCreateCommandPool(device->device(), &createInfo, nullptr, &_commandPool) == VK_SUCCESS, "Failed to create Vulkan command pool");

	Logger::log_info("Successfully created Vulkan command pool at ", get_address(this), "!", Logger::end_l());
}

void VulkanCommandPool::reset() {
	lassert(vkResetCommandPool(device->device(), _commandPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT), "Failed to reset command pool!");
}

// command buffer
CommandBufferManager::VulkanCommandBuffer::~VulkanCommandBuffer() noexcept {
	vkFreeCommandBuffers(device->device(), commandPool->commandPool(), 1, &commandBuffer);

	Logger::log_info("Successfully destroyed a Vulkan command buffer!");
}

void CommandBufferManager::VulkanCommandBuffer::create(const VulkanDevice* const device, const VulkanCommandPool* const commandPool, const VkCommandBufferLevel level) {
	Logger::log_debug(Logger::tab(), "Creating Vulkan command buffer...");

	this->commandPool = commandPool;
	this->device = device;

	// locate the memory
	VkCommandBufferAllocateInfo allocInfo{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		nullptr,
		commandPool->commandPool(),
		level,
		1
	};

	// create the command buffers
	lassert(vkAllocateCommandBuffers(device->device(), &allocInfo, &commandBuffer) == VK_SUCCESS, "Failed to create Vulkan command buffer!");

	Logger::log_debug(Logger::tab(), "Successfully created Vulkan command buffer at ", get_address(this), "!", Logger::end_l());
}

// manager
CommandBufferManager::~CommandBufferManager() noexcept {
	_commandBuffers.clear();

	Logger::log_info("Successfully destroyed a command buffer manager!");
}

void CommandBufferManager::create(const VulkanDevice* const device, const VulkanCommandPool* const commandPool, const VkCommandBufferLevel level) {
	Logger::log_info("Creating command buffer manager...");

	_commandBuffers.reserve(Settings::Memory::maxCommandBuffers);
	_unused.reserve(Settings::Memory::maxCommandBuffers);
	_inUse.reserve(Settings::Memory::maxCommandBuffers);
	_commandBuffers.resize(Settings::Memory::maxCommandBuffers);

	for (uint32 i = 0; i < Settings::Memory::maxCommandBuffers; i++) {
		_commandBuffers.at(i).create(device, commandPool, level);
		_unused.push_back(i);
	}

	Logger::log_info("Successfully created command buffer manager at ", get_address(this), "!", Logger::end_l());
}

void CommandBufferManager::begin(const CommandBuffer cmdBuffer, const VkCommandBufferUsageFlags usage) {
	const auto& index = std::distance(_unused.begin(), std::find(_unused.begin(), _unused.end(), cmdBuffer)); // find the command buffer that has been used
	move_element(_unused, _inUse, index); // set that command buffer as in use

	// some info about the recording
	VkCommandBufferBeginInfo beginInfo{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		nullptr,
		usage,
		nullptr
	};

	// start recording
	lassert(vkBeginCommandBuffer(_commandBuffers.at(cmdBuffer).commandBuffer, &beginInfo) == VK_SUCCESS, "Failed to start recording Vulkan command buffer!");
}

void CommandBufferManager::end(const CommandBuffer cmdBuffer) const {
	lassert(vkEndCommandBuffer(_commandBuffers.at(cmdBuffer).commandBuffer) == VK_SUCCESS, "Failed to stop recording command buffer!");
}

void CommandBufferManager::reset(const CommandBuffer cmdBuffer, const VkCommandBufferResetFlags flags) {
	const auto& index = std::distance(_inUse.begin(), std::find(_inUse.begin(), _inUse.end(), cmdBuffer)); // find the command buffer that has been used
	move_element(_inUse, _unused, index); // set that command buffer as unused
	lassert(vkResetCommandBuffer(_commandBuffers.at(cmdBuffer).commandBuffer, flags) == VK_SUCCESS, "Failed to reset command buffer!"); // reset the command buffer
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
		&_commandBuffers.at(cmdBuffer).commandBuffer,
		0,
		nullptr
	};

	// submit the queue
	lassert(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE) == VK_SUCCESS, "Failed to submit Vulkan queue!");
}

void CommandBufferManager::wait_queue(const CommandBuffer cmdBuffer, const VkQueue queue) const {
	lassert(vkQueueWaitIdle(queue) == VK_SUCCESS, "Failed to wait for device queue!");
}

} // namespace lyra
