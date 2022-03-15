#include <rendering/vulkan/GPU_buffer.h>

namespace lyra {

VulkanGPUBuffer::VulkanGPUBuffer() {}

void VulkanGPUBuffer::destroy() {
    vmaDestroyBuffer(device->get().allocator, var.buffer, var.memory);

    delete device;

    LOG_INFO("Succesfully destroyed Vulkan GPU buffer!")
}

void VulkanGPUBuffer::create(VulkanDevice device, VkDeviceSize size, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memUsage) {
    LOG_INFO("Creating Vulkan GPU memory buffer...")
    
    var.bufferUsage = bufferUsage;
    var.memUsage = memUsage;
    var.size = size;

    // buffer creation info
    VkBufferCreateInfo 		bufferInfo {
        VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        nullptr,
        0,
        size,
        bufferUsage,
        VK_SHARING_MODE_EXCLUSIVE,
        0,
        0
    };

    // memory allocation info
    VmaAllocationCreateInfo memAllocInfo = {
        0,
        memUsage,
        0,
        0,
        0,
        nullptr,
        nullptr,
        0
    }; 

    vmaCreateBuffer(device.get().allocator, &bufferInfo, &memAllocInfo, &var.buffer, &var.memory, nullptr);

    vmaBindBufferMemory(device.get().allocator, var.memory, var.buffer);

    LOG_INFO("Succesfully created Vulkan GPU buffer at ", GET_ADDRESS(this), "!", END_L)
}

void VulkanGPUBuffer::copy(const VulkanCommandPool commandPool, const VulkanGPUBuffer srcBuffer) {
    // create a temporary command buffer
    VulkanCommandBuffer 		commandBuffer;

    // create the command buffer
    commandBuffer.create(*device, commandPool);

    // start recording
    commandBuffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    // transfer the contents of the sorce to the destination buffer
    VkBufferCopy 				copyRegion {
        0,
        0,
        var.size
    };

    vkCmdCopyBuffer(commandBuffer.get(), srcBuffer.get().buffer, var.buffer, 1, &copyRegion);

    commandBuffer.end();		// end recording

    VkSubmitInfo 				submitInfo {
        VK_STRUCTURE_TYPE_SUBMIT_INFO,
        nullptr,
        0,
        nullptr,
        nullptr,
        1,
        commandBuffer.get_ptr(),
        0,
        nullptr
    };

    vkQueueSubmit(device->get().graphicsQueue.queue, 1, &submitInfo, VK_NULL_HANDLE);   // @todo maybe move these functions into the device?
    vkQueueWaitIdle(device->get().graphicsQueue.queue);		// wait for completion

    commandBuffer.destroy();		    // delete command buffer after copying

    LOG_INFO("Succesfully copied Vulkan GPU buffer at ", GET_ADDRESS(&srcBuffer), "to ", GET_ADDRESS(this), "!", END_L)
}

void VulkanGPUBuffer::map_memory(void* data) {
    if(vmaMapMemory(device->get().allocator, var.memory, &data) != VK_SUCCESS) LOG_EXEPTION("Failed to map buffer memory at ", GET_ADDRESS(var.memory))
}

void VulkanGPUBuffer::unmap_memory() const {
    vmaUnmapMemory(device->get().allocator, var.memory);
}

VulkanGPUBuffer::Variables VulkanGPUBuffer::get() const {
    return var;
}

} // namespace lyra
