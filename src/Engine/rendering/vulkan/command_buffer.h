/*************************
 * @file command_buffer.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief a wrapper around the Vulkan command buffers
 * 
 * @date 2022-02-05
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <core/defines.h>
#include <rendering/vulkan/devices.h>
#include <rendering/vulkan/command_buffer.h>
#include <core/logger.h>

#include <SDL.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief command pool
 */
struct VulkanCommandPool {
public:

    VulkanCommandPool();

    /**
     * @brief destroy the VulkanCommandBuffer object
     * @brief it works just like the destructor, but it can be called anywhere
     */
    void                    destroy();

    /**
     * @brief create a Vulkan command pool to allocate the command buffers
     * 
     * @param device device
     */
    void                    create(VulkanDevice device);
    
    /**
     * @brief get the command pool
     * 
     * @return VkCommandPool 
     */
    VkCommandPool           get() const;
    /**
     * @brief get the command pool as a pointer
     * 
     * @return VkCommandPool 
     */
    const VkCommandPool*    get_ptr() const;

private:
    VkCommandPool           commandPool;

    VulkanDevice*           device;
};

/**
 * @brief command buffer
 */
struct VulkanCommandBuffer {
public:

    VulkanCommandBuffer();

    /**
     * @brief destroy the VulkanCommandBuffer object
     * @brief it works just like the destructor, but it can be called anywhere
     */	
    void                    destroy();

    /**
     * @brief create the Vulkan command buffers
     * 
     * @param device device
     * @param commandPool command pool
     * @param level level of the command buffer
     */
    void                    create(VulkanDevice device, VulkanCommandPool commandPool, const VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    /**
     * @brief begin recording a commandBuffer
     * 
     * @param usage what the recording will be used for
     */
    void                    begin(const VkCommandBufferUsageFlags usage = 0) const;
    /**
     * @brief end recording a commandBuffer
     */
    void                    end() const;
    /**
     * reset the command buffer after everything has been recorded
     * 
     * @flags additional flags
     */
    void                    reset(VkCommandBufferResetFlags flags = 0) const;

    /**
     * @brief get the command buffer
     * 
     * @return VkCommandBuffer
     */
    VkCommandBuffer         get() const;

    /**
     * @brief get the command buffer as a pointer
     * 
     * @return const VkCommandBuffer* 
     */
    const VkCommandBuffer* 	get_ptr() const;

private:
    VkCommandBuffer         commandBuffer;

    VulkanDevice*           device;
    VulkanCommandPool*      commandPool;
};

} // namespace lyra
