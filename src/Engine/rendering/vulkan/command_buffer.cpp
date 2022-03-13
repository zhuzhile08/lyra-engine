#include <rendering/vulkan/command_buffer.h>

namespace lyra {

// command pool
VulkanCommandPool::VulkanCommandPool() { }

void VulkanCommandPool::destroy() {
    vkDestroyCommandPool(device->get().device, commandPool, nullptr);

    delete device;

    LOG_INFO("Succesfully destroyed Vulkan command pool!")
}

void VulkanCommandPool::create(VulkanDevice device) {
    this->device = &device;

    VkCommandPoolCreateInfo createInfo {
        VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        nullptr,
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        device.get().graphicsQueue.familyIndex
    };

    if(vkCreateCommandPool(device.get().device, &createInfo, nullptr, &commandPool) != VK_SUCCESS) LOG_EXEPTION("Failed to create Vulkan command pool")

    LOG_INFO("Succesfully created Vulkan command pool at ", GET_ADDRESS(this), "!", END_L)
}

VkCommandPool VulkanCommandPool::get() const {
    return commandPool;
}

const VkCommandPool* VulkanCommandPool::get_ptr() const {
    return &commandPool;
}

// command buffer
VulkanCommandBuffer::VulkanCommandBuffer() { }

void VulkanCommandBuffer::destroy() {
	vkFreeCommandBuffers(device->get().device, commandPool->get(), 1, &commandBuffer);
    delete device;
    delete commandPool;

    LOG_INFO("Succesfully destroyed Vulkan command buffer!")
}

void VulkanCommandBuffer::create(VulkanDevice device, VulkanCommandPool commandPool, const VkCommandBufferLevel level) {
    this->commandPool = &commandPool;
    this->device = &device;

    // locate the memory
    VkCommandBufferAllocateInfo allocInfo {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        nullptr,
        commandPool.get(),
        VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        1
    };

    // create the command buffers
    if(vkAllocateCommandBuffers(device.get().device, &allocInfo, &commandBuffer) != VK_SUCCESS) LOG_EXEPTION("Failed to create Vulkan command buffer")

    LOG_INFO("Succesfully created Vulkan command buffer at ", GET_ADDRESS(this), "!", END_L)
}

void VulkanCommandBuffer::begin(const VkCommandBufferUsageFlags usage) const {
    // some info about the recording
	VkCommandBufferBeginInfo beginInfo {
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		nullptr,
		usage,
		nullptr
	};

    // start recording
	if(vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) LOG_EXEPTION("Failed to start recording Vulkan command buffer")
}

void VulkanCommandBuffer::end() const {
	if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) LOG_EXEPTION("Failed to stop recording command buffer")
}

VkCommandBuffer VulkanCommandBuffer::get() const {
    return commandBuffer;
}

const VkCommandBuffer* VulkanCommandBuffer::get_ptr() const {
    return &commandBuffer;
}

} // namespace lyra
