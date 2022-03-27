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
public:
	RenderStage();
	/**
	 * @brief destroy the renderer
	 */
	void destroy() noexcept;
	/**
	 * @brief create the renderer
	 *
	 * @param renderer the renderer
	 */
	void create(Renderer* renderer);

	/**
	 * @brief bind the functions for resetting and finish recording the command buffers
	 *
	 * @param renderer the renderer
	 */
	void draw() noexcept;

	/**
	 * @brief get the bind queue
	 * 
	 * @return const CallQueue
	*/
	const CallQueue bind_queue() const noexcept { return _bind_queue; }
	/**
	 * @brief get the draw queue
	 * 
	 * @return const CallQueue
	*/
	const CallQueue draw_queue() const noexcept { return _draw_queue; }

private:
	VulkanFramebuffers _framebuffers;
	std::vector<VulkanCommandBuffer> _commandBuffers;
	VulkanGraphicsPipeline _pipeline;

	CallQueue _bind_queue;
	CallQueue _draw_queue;

	bool _drawing = true;

	Renderer* renderer;

	/**
	 * @brief bind a descriptor set
	 *
	 * @param descriptor descriptor
	 * @param pipelineLayout pipeline layout
	 */
	void bind_descriptor(const VulkanDescriptor descriptor) const noexcept;

	void push_constants() const noexcept;
	/**
	 * @brief begin render passes
	 *
	 * @param framebuffers frame buffer
	 * @param index index of the frame buffer
	 */
	void begin_render_pass(const VkRenderPassBeginInfo beginInfo) const noexcept;
	/**
	 * @brief end render passes
	 *
	 * @param framebuffers frame buffer
	 */
	void end_render_pass() const noexcept;
	/**
	 * @brief bind the graphics pipeline
	 *
	 * @param pipeline pipeline
	 */
	void bind_pipeline() const noexcept;
	/**
	 * @brief bind a model
	 *
	 * @param vertexBuffer vertex buffer of the model
	 * @param indexBuffer index buffer of the model
	 * @param cmdBuffIndex index of the command buffer to bind the model
	 */
	void bind_model(const VkBuffer vertexBuffer, const VkBuffer indexBuffer) const noexcept;
	/**
	 * @brief draw a model
	 *
	 * @param size size of the index buffer
	 * @param cmdBuffIndex index of the command buffer to draw the model
	 */
	void draw_model(const uint32 size) const noexcept;

	/**
	 * @brief record all the commands
	 */
	void record_command_buffers();

	friend class Mesh;
	friend class Texture;
};

}
