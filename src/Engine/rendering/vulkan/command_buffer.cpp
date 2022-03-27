#include <rendering/vulkan/command_buffer.h>

namespace lyra {

// command pool
VulkanCommandPool::VulkanCommandPool() { }

void VulkanCommandPool::destroy() noexcept {
	vkDestroyCommandPool(device->device(), commandPool, nullptr);

	delete device;

	LOG_INFO("Succesfully destroyed Vulkan command pool!")
}

void VulkanCommandPool::create(const VulkanDevice* device) {
	LOG_INFO("Creating Vulkan command pool...")

	this->device = device;

	VkCommandPoolCreateInfo createInfo{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		nullptr,
		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		device->graphicsQueue().familyIndex
	};

	if (vkCreateCommandPool(device->device(), &createInfo, nullptr, &commandPool) != VK_SUCCESS) LOG_EXEPTION("Failed to create Vulkan command pool")

	LOG_INFO("Succesfully created Vulkan command pool at ", GET_ADDRESS(this), "!", END_L)
}

// command buffer
VulkanCommandBuffer::VulkanCommandBuffer() { }

void VulkanCommandBuffer::destroy() noexcept {
	vkFreeCommandBuffers(device->device(), commandPool->get(), 1, &commandBuffer);

	LOG_INFO("Succesfully destroyed Vulkan command buffer!")
}

void VulkanCommandBuffer::create(const VulkanDevice* device, const VulkanCommandPool* commandPool, const VkCommandBufferLevel level) {
	LOG_INFO("Creating Vulkan command buffer...")

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
	if (vkAllocateCommandBuffers(device->device(), &allocInfo, &commandBuffer) != VK_SUCCESS) LOG_EXEPTION("Failed to create Vulkan command buffer!")

	LOG_INFO("Succesfully created Vulkan command buffer at ", GET_ADDRESS(this), "!", END_L)
}

void VulkanCommandBuffer::begin(const VkCommandBufferUsageFlags usage) const {
	// some info about the recording
	VkCommandBufferBeginInfo beginInfo{
		VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		nullptr,
		usage,
		nullptr
	};

	// start recording
	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) LOG_EXEPTION("Failed to start recording Vulkan command buffer!")
}

void VulkanCommandBuffer::end() const {
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) LOG_EXEPTION("Failed to stop recording command buffer!")
}

void VulkanCommandBuffer::reset(VkCommandBufferResetFlags flags) const {
	if (vkResetCommandBuffer(commandBuffer, flags) != VK_SUCCESS) LOG_EXEPTION("Failed to reset command buffer!")
}

void VulkanCommandBuffer::submit_queue(const VkQueue queue) const {
	// queue submission info
	VkSubmitInfo submitInfo = {
		VK_STRUCTURE_TYPE_SUBMIT_INFO,
		nullptr,
		0,
		nullptr,
		nullptr,
		1,
		&commandBuffer,
		0,
		nullptr
	};

	// submit the queue
	if (vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) LOG_EXEPTION("Failed to submit Vulkan queue!");
}

void VulkanCommandBuffer::wait_queue(const VkQueue queue) const {
	if (vkQueueWaitIdle(queue) != VK_SUCCESS) LOG_EXEPTION("Failed to wait for device queue!")
}

} // namespace lyra
