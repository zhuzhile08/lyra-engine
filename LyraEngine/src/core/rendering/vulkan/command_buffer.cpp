#include <core/rendering/vulkan/command_buffer.h>

namespace lyra {

// command buffer
void CommandBufferManager::VulkanCommandBuffer::create(const VulkanDevice* const device, const VulkanCommandPool* const commandPool, const VkCommandBufferLevel level) {
	Logger::log_info("Creating Vulkan command buffer...");

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

void CommandBufferManager::create(const VulkanDevice* const device, const VulkanCommandPool* const commandPool, const VkCommandBufferLevel level) {
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
	int index = std::distance(_unused.begin(), std::find(_unused.begin(), _unused.end(), cmdBuffer)); // find the command buffer that has been used
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

void CommandBufferManager::reset(const CommandBuffer cmdBuffer, const VkCommandBufferResetFlags flags) {
	int index = std::distance(_inUse.begin(), std::find(_inUse.begin(), _inUse.end(), cmdBuffer)); // find the command buffer that has been used
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

#ifdef _DEBUG
	Logger::log_debug(Logger::tab(), "Submitted command buffer at: ", get_address(this));
#endif
}

} // namespace lyra
