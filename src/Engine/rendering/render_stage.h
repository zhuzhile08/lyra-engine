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
#include <rendering/vulkan/pipeline.h>
#include <rendering/vulkan/command_buffer.h>
#include <rendering/vulkan/descriptor.h>
#include <rendering/vulkan/framebuffer.h>
#include <rendering/renderer.h>

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
class RenderStage : noud::Node {
private:
    /**
     * @brief struct containing all the variabels
     */
    struct          Variables {
        VulkanFramebuffers                  framebuffers;
        VulkanDescriptor                    descriptors;
        std::vector<VulkanCommandBuffer>    commandBuffers;
        VulkanGraphicsPipeline              pipeline;

        CallQueue                           bind_queue;
        CallQueue                           draw_queue;

        bool                                drawing = true;
    };

public:
    RenderStage();
    /**
     * @brief destroy the renderer
     */
    void            destroy();
    /**
     * @brief create the renderer
     *
     * @param renderer the renderer
     */
    void            create(Renderer renderer, uint16 descriptorCount, VulkanDescriptor::Writer writer);
    
    /**
     * @brief bind the functions for resetting and finish recording the command buffers
     * 
     * @param renderer the renderer
     */
    void            draw(Renderer renderer);

private:
    Variables       var;

    /**
     * @brief bind a descriptor set
     *
     * @param descriptor descriptor
     * @param pipelineLayout pipeline layout
     */
    void            bind_descriptor(const VulkanDescriptor descriptor, const VulkanGraphicsPipeline pipeline, int cmdBuffIndex) const;
    
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
     */
    void            bind_pipeline(const VulkanGraphicsPipeline pipeline, int cmdBuffIndex) const;
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
    void            record_command_buffers(int index, int currentFrame);

    friend class    Mesh;
    friend class    Texture;
};

}
