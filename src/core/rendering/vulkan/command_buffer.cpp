#include <core/rendering/vulkan/command_buffer.h>

namespace lyra {

// command pool
VulkanCommandPool::VulkanCommandPool() { }

VulkanCommandPool::~VulkanCommandPool() {
	vkDestroyCommandPool(device->device(), commandPool, nullptr);

	Logger::log_info("Succesfully destroyed Vulkan command pool!");
}

void VulkanCommandPool::destroy() noexcept {
	this->~VulkanCommandPool();
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

	if (vkCreateCommandPool(device->device(), &createInfo, nullptr, &commandPool) != VK_SUCCESS) Logger::log_exception("Failed to create Vulkan command pool");

	Logger::log_info("Succesfully created Vulkan command pool at ", get_address(this), "!", Logger::end_l());
}

// command buffer
VulkanCommandBuffer::VulkanCommandBuffer() { }

VulkanCommandBuffer::~VulkanCommandBuffer() {
	vkFreeCommandBuffers(device->device(), commandPool->get(), 1, &commandBuffer);

	Logger::log_info("Succesfully destroyed Vulkan command buffer!");
}

void VulkanCommandBuffer::destroy() noexcept {
	this->~VulkanCommandBuffer();
}

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

	Logger::log_info("Succesfully created Vulkan command buffer at ", get_address(this), "!", Logger::end_l());
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
	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) Logger::log_exception("Failed to start recording Vulkan command buffer!");

	Logger::log_debug(Logger::tab(), "Began recording command buffer at: ", get_address(this));
}

void VulkanCommandBuffer::end() const {
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) Logger::log_exception("Failed to stop recording command buffer!");

	Logger::log_debug(Logger::tab(), "End recording command buffer at: ", get_address(this));
}

void VulkanCommandBuffer::reset(const VkCommandBufferResetFlags flags) const {
	if (vkResetCommandBuffer(commandBuffer, flags) != VK_SUCCESS) Logger::log_exception("Failed to reset command buffer!");

	Logger::log_debug(Logger::tab(), "Reset command buffer at: ", get_address(this));
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
	if (vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) Logger::log_exception("Failed to submit Vulkan queue!");

	Logger::log_debug(Logger::tab(), "Submitted command buffer at: ", get_address(this));
}

void VulkanCommandBuffer::wait_queue(const VkQueue queue) const {
	if (vkQueueWaitIdle(queue) != VK_SUCCESS) Logger::log_exception("Failed to wait for device queue!");
}

void VulkanCommandBuffer::pipeline_barrier(
	const VkPipelineStageFlags srcStageFlags,
	const VkPipelineStageFlags dstStageFlags,
	const VkMemoryBarrier* const memory,
	const VkBufferMemoryBarrier* const buffer,
	const VkImageMemoryBarrier* const image,
	const VkDependencyFlags dependency
) const {
	vkCmdPipelineBarrier(
		commandBuffer,
		srcStageFlags,
		dstStageFlags,
		dependency,
		(memory == nullptr) ? 0 : 1,
		memory,
		(buffer == nullptr) ? 0 : 1,
		buffer,
		(image == nullptr) ? 0 : 1,
		image
	);

	Logger::log_debug(Logger::tab(), "Setup a pipeline barrier with the command buffer at: ", get_address(this));
}

} // namespace lyra