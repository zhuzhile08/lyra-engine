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
#include <rendering/vulkan/framebuffer.h>
#include <rendering/vulkan/sync_objects.h>
#include <rendering/vulkan/instance.h>
#include <rendering/vulkan/swapchain.h>
#include <rendering/window.h>

#include <core/logger.h>

#include <memory>
#include <vector>

#include <vulkan/vulkan.h>

namespace lyra { 

/**
 * @brief a vulkan renderer with basic features
 * @todo maybe abstract vma memory allocation?
 */
class Renderer {
private:
    /**
     * @brief struct containing all the variabels
     */
    struct          Variables {
        VulkanInstance                      instance;
        VulkanDevice                        device;
        VulkanCommandPool                   commandPool;
        VulkanSwapchain                     swapchain;
        VulkanFramebuffers                  framebuffers;
        VulkanDescriptorSetLayout           descriptorSetLayout;
        /// @todo textures samplers, and other stuff to do
        VulkanDescriptorPool                descriptorPool;
        std::vector<VulkanDescriptor>       descriptors;
        std::vector<VulkanCommandBuffer>    commandBuffers;
        VulkanSyncObjects                   syncObjects;

        uint8                               currentFrame;

        CallQueue                           scene_queue;
        CallQueue                           bind_queue;
        CallQueue                           descriptor_queue;
        CallQueue                           draw_queue;

        bool                                drawing = true;
    };

public:
    Renderer();
    /**
     * @brief destroy the renderer
     */
    void            destroy();
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
     * @brief recreate the swapchain and related stuff in case of some events
     */
    void            recreate_swapchain();
    /**
     * @brief destroy the swapchain and related stuff in case of some events
     */
    void            destroy_swapchain();

    /// @todo queue and multithread this function
    void load_scene();
    void unload_scene();
    
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
        const VulkanCommandBuffer               commandBuffer   = VulkanCommandBuffer(),
        const VulkanSyncObjects                 syncObjects     = VulkanSyncObjects(),
        const uint32                            syncObjectIndex = 0,
        const VkPipelineStageFlags              stageFlags      = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    ) const;

    /**
     * @brief wait for queue to finish submitting
     * 
     * @param queue queue to wait for
     */
    void            wait_device_queue(const VulkanDevice::VulkanQueueFamily queue) const;

private:
    Variables       var;

    Window*         window;

    /**
     * @brief update the current frame count (increment to MAX_FRAMES_IN_FLIGHT and loop around)
     */
    void            update_frame_count();

    /**
     * @brief overloaded Vulkan queue submitting function, only for after every commandbuffer has finisched recording
     *
     * @param queue queue to submit
     * @param index index of the synchronisation objects and commandbuffers
     * @param stageFlags pipeline shader stage flags
     */
    void            submit_device_queue(const VulkanDevice::VulkanQueueFamily queue, const VkPipelineStageFlags stageFlags) const;
    /**
     * @brief presenting the device queues
     * 
     * @param imageIndex index of the current image
     * 
     * @return VkResult
     */
    void            present_queue(const uint32 imageIndex);

    /**
     * @brief bind a descriptor set
     * 
     * @param descriptor descriptor
     * @param pipelineLayout pipeline layout
    
    void            bind_descriptor(const VulkanDescriptor descriptor, const VulkanGraphicsPipeline pipeline, int cmdBuffIndex) const;
     */
    void            push_constants() const;
    /**
     * @brief begin render passes
     * 
     * @param framebuffers frame buffer
     * @param index index of the frame buffer
     */
    void            begin_render_pass(int cmdBuffIndex, const VkRenderPassBeginInfo beginInfo) const;
    /**
     * @brief end render passes
     * 
     * @param framebuffers frame buffer
     */
    void            end_render_pass(int cmdBuffIndex) const;
    /**
     * @brief bind the graphics pipeline
     * 
     * @param pipeline pipeline
    
    void            bind_pipeline(const VulkanGraphicsPipeline pipeline, int cmdBuffIndex) const;
     */
    /**
     * @brief bind a model
     * 
     * @param vertexBuffer vertex buffer of the model
     * @param indexBuffer index buffer of the model
     * @param cmdBuffIndex index of the command buffer to bind the model
     */
    void            bind_model(const VkBuffer vertexBuffer, const VkBuffer indexBuffer, int cmdBuffIndex) const;
    /**
     * @brief draw a model
     * 
     * @param size size of the index buffer
     * @param cmdBuffIndex index of the command buffer to draw the model
     */
    void            draw_model(const uint32 size, int cmdBuffIndex) const;

    /**
     * @brief record all the commands
     */
    void            record_command_buffers(int index);
    /**
     * @brief take the recorded commands and draw everything
     */
    void            draw();

    friend class    Mesh;
    friend class    Texture;
};

} // namespace Vulkan
