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
#include <res/loaders/load_file.h>
#include <res/mesh.h>
#include <rendering/vulkan_wrappers.h>
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
    struct                              Variables {
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

        CallQueue                           draw_queue;

        bool                                drawing = true;
    };

public:
    Renderer();
    /**
     * @brief destroy the renderer
     */
    void                                destroy();
    /**
     * @brief create the renderer
     * 
     * @param window the window
     */
    void                                create(Window window);

    /**
     * @brief execute all the commands and update everything 
     */
    void                                draw();
    
    /**
     * @brief the main loop. this is called every frame until drawing is set to false
     */
    void                                update();

    /**
     * @brief recreate the swapchain and related stuff in case of some events
     */
    void                                recreate_swapchain();
    /**
     * @brief destroy the swapchain and related stuff in case of some events
     */
    void                                destroy_swapchain();

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
    void                                submit_device_queue(
        VulkanDevice::VulkanQueueFamily queue, 
        VulkanCommandBuffer             commandBuffer   = VulkanCommandBuffer(),
        VulkanSyncObjects               syncObjects     = VulkanSyncObjects(), 
        uint32                          syncObjectIndex = 0,
        VkPipelineStageFlags            stageFlags      = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    );
    /**
     * @brief overloaded Vulkan queue submitting function, only for after every commandbuffer has finisched recording
     * 
     * @param queue queue to submit
     * @param index index of the synchronisation objects and commandbuffers
     * @param stageFlags pipeline shader stage flags
     */
    void                                submit_device_queue(VulkanDevice::VulkanQueueFamily queue, uint32 index, VkPipelineStageFlags stageFlags);
    /**
     * @brief wait for queue to finish submitting
     * 
     * @param queue queue to wait for
     */
    void                                wait_device_queue(VulkanDevice::VulkanQueueFamily queue);

private:
    Variables                           var;

    Window*                             window;

    /**
     * @brief bind a descriptor set
     * 
     * @param descriptor descriptor
     * @param pipelineLayout pipeline layout
     */
    void                                bind_descriptor(const VulkanDescriptor descriptor, const VulkanGraphicsPipeline pipeline, const int cmdBuffIndex) const;
    void                                push_constants() const;
    /**
     * @brief begin render passes
     * 
     * @param framebuffers frame buffer
     * @param index index of the frame buffer
     */
    void                                begin_render_pass(const VulkanFramebuffers framebuffer, const int index, const VkClearValue clear, const int cmdBuffIndex) const;
    /**
     * @brief end render passes
     * 
     * @param framebuffers frame buffer
     */
    void                                end_render_pass(const int cmdBuffIndex) const;
    /**
     * @brief bind the graphics pipeline
     * 
     * @param pipeline pipeline
     */
    void                                bind_pipeline(const VulkanGraphicsPipeline pipeline, const int cmdBuffIndex) const;

    friend class                        Mesh;
    friend class                        Texture;
};

} // namespace Vulkan
