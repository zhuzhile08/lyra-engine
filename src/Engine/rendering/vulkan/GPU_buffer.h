/*************************
 * @file GPU_buffer.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief wrapper around the Vulkan buffers
 * 
 * @date 2022-02-05
 * 
 * @copyright Copyright (c) 2022 Zhile Zhu
 *************************/

#pragma once

#include <core/defines.h>
#include <rendering/vulkan/devices.h>
#include <rendering/vulkan/command_buffer.h>
#include <core/logger.h>

#include <memory>
#include <cstring>
#include <vector>

#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief wrapper around the Vulkan buffer
 */
class VulkanGPUBuffer {
private:
    struct          Variables {
        VkBuffer            buffer;
        VmaAllocation       memory;
        VkDeviceSize        size;
        VkBufferUsageFlags  bufferUsage;
        VmaMemoryUsage      memUsage;
    };

public:
    VulkanGPUBuffer();

    /**
     * @brief destroy the VulkanGPUBuffer object
     */
    void             destroy() noexcept;

    /**
     * @brief create the buffer
     * 
     * @param device device
     */
    void             create(VulkanDevice device, VkDeviceSize size, VkBufferUsageFlags bufferUsage, VmaMemoryUsage memUsage);

    /**
     * @brief copy a buffer to another
     * 
     * @param device device
     * @param commandBuffer command buffers
     * @param srcBuffer the buffer to copy
     * @param size the size of the buffer
     * @return true 
     */
    void             copy(const VulkanCommandPool commandPool, const VulkanGPUBuffer srcBuffer);

    /**
     * @brief map GPU memory to normal memory, copy some stuff in there and unmap it
     * 
     * @param src data to copy into the buffer
     */
    void             copy_data(void* src);

    /**
     * @brief get the buffer
     * 
     * @return Variables
     */
    Variables        get() const noexcept;

private:
    Variables        var;

    VulkanDevice*    device;
};

} // namespace lyra
