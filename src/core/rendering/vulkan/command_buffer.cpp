#include <core/rendering/vulkan/command_buffer.h>

namespace lyra {

// command pool
VulkanCommandPool::VulkanCommandPool() { }


void VulkanCommandPool::create(const VulkanDevice* const device) {
	Logger::log_info("Creating Vulkan command pool...");

	this->device = device;

	VkCommandPoolCreateInfo createInfo{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		nullptr,
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		device->graphicsQueue().familyIndex
	};

	if (vkCreateCommandPool(device->device(), &createInfo, nullptr, &commandPool) != VK_SUCCESS) Logger::log_exception("Failed to create Vulkan command pool");

	Logger::log_info("Successfully created Vulkan command pool at ", get_address(this), "!", Logger::end_l());
}

// command buffer
VulkanCommandBuffer::VulkanCommandBuffer() { }

void VulkanCommandBuffer::create(const VulkanDevice* const device, const VulkanCommandPool* const commandPool, const VkCommandBufferLevel level) {
	Logger::log_info("Creating Vulkan command buffer...");

	this->commandPool = commandPool;
	this->device = device;

	// locate the memory
	VkCommandBufferAllocateInfo allocInfo{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		nullptr,
		commandPool->get(),
		VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		1
	};

	// create the command buffers
	if (vkAllocateCommandBuffers(device->device(), &allocInfo, &commandBuffer) != VK_SUCCESS) Logger::log_exception("Failed to create Vulkan command buffer!");

	Logger::log_info("Successfully created Vulkan command buffer at ", get_address(this), "!", Logger::end_l());
}

} // namespace lyra