#include <core/rendering/vulkan/command_pool.h>

namespace lyra {

// command pool
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

} // namespace lyra
