/*************************
 * @file renderer.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief a rendering system around the Vulkan API with basic features
 * 
 * @date 2022-02-04
 * 
 * @copyright Copyright (c) 2022 Zhile Zhu
 *************************/

#pragma once

#include <core/defines.h>
#include <core/queue_types.h>
#include <rendering/vulkan/command_buffer.h>
#include <rendering/vulkan/descriptor.h>
#include <rendering/vulkan/devices.h>
#include <rendering/vulkan/sync_objects.h>
#include <rendering/vulkan/instance.h>
#include <rendering/vulkan/swapchain.h>
#include <rendering/window.h>

#include <core/logger.h>

#include <memory>
#include <vector>

#include <noud.h>
#include <vulkan/vulkan.h>

namespace lyra { 

/**
 * @brief a vulkan renderer with basic features
 * @todo maybe abstract vma memory allocation?
 */
class Renderer : noud::Node {
private:
    /**
     * @brief struct containing all the variabels
     */
    struct          Variables {
        VulkanInstance                      instance;
        VulkanDevice                        device;
        VulkanCommandPool                   commandPool;
        VulkanSwapchain                     swapchain;
        VulkanDescriptorSetLayout           descriptorSetLayout;
        VulkanDescriptorPool                descriptorPool;
        std::vector<VulkanDescriptor>       descriptors;
        VulkanSyncObjects                   syncObjects;

        CallQueue                           renderQueue;
        CallQueue                           submitQueue;

        bool                                running = true;

        uint8                               currentFrame = 0;
        uint32                              imageIndex;
    };

public:
    Renderer();
    /**
     * @brief destroy the renderer
     */
    void            destroy() noexcept;
    /**
     * @brief create the renderer
     * 
     * @param window the window
     */
    void            create(Window window);
    
    /**
     * @brief the main loop. this is called every frame until drawing is set to false
     */
    void            update();
    
    /**
     * @brief submit a Vulkan queue after command queue recording
     * 
     * @param queue the queue to submit
     * @param commandBuffer recorded commandbuffer
     * @param syncObjects synchronisation objects (only needed if every command has finished recording)
     * @param syncObjectIndex index of the synchronisation objects to use
     * @param stageFlags pipeline shader stage flags
     */
    void            submit_device_queue(
        const VulkanDevice::VulkanQueueFamily   queue,
        const VulkanCommandBuffer               commandBuffer
    ) const;

    /**
     * @brief submit a Vulkan queue after command queue recording
     *
     * @param queue the queue to submit
     * @param commandBuffer recorded commandbuffer
     * @param stageFlags pipeline shader stage flags
     */
    void            submit_device_queue(const VulkanDevice::VulkanQueueFamily queue, const VulkanCommandBuffer commandBuffer, const VkPipelineStageFlags stageFlags) const;

    /**
     * @brief wait for queue to finish submitting
     * 
     * @param queue queue to wait for
     */
    void            wait_device_queue(const VulkanDevice::VulkanQueueFamily queue) const;

    Variables       get() const noexcept;

private:
    Variables       var;

    Window*         window;

    /**
     * @brief recreate the swapchain and related stuff in case of some events
     */
    void            recreate_swapchain();
    /**
     * @brief destroy the swapchain and related stuff in case of some events
     */
    void            destroy_swapchain() noexcept;

    /**
     * @brief present all the recorded commands
     * 
     * @param index of the images
     */
    void            present_queue(const uint32 imageIndex);
    /**
     * @brief take the recorded commands and draw everything
     */
    void            draw();
    /**
     * @brief update the frame count
     */
    void            update_frame_count() noexcept;

    friend class    RenderStage;
};

} // namespace Vulkan
