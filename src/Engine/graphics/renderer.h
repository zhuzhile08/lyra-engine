/*************************
 * @file context.h
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
#include <graphics/pipeline.h>
#include <core/rendering/vulkan/command_buffer.h>
#include <core/rendering/vulkan/descriptor.h>
#include <core/rendering/vulkan/framebuffer.h>
#include <core/rendering/context.h>

#include <core/logger.h>
#include <vector>

#include <noud.h>
#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief a vulkan context with basic features
 * @todo maybe abstract vma memory allocation?
 */
class Renderer {
public:
	Renderer();

	/**
	 * @brief destructor of the renderer
	 */
	virtual ~Renderer() noexcept;

	/**
	 * @brief destroy the renderer
	 */
	void destroy() noexcept;

	Renderer operator=(const Renderer&) = delete;

	/**
	 * @brief create the renderer
	 *
	 * @param context the context
	 */
	void create(Context* context);

	/**
	 * @brief bind the functions for resetting and finish recording the command buffers
	 *
	 * @param context the context
	 */
	void draw() noexcept;

	/**
	 * @brief get the draw queue
	 * 
	 * @return const CallQueue
	*/
	const CallQueue draw_queue() const noexcept { return _draw_queue; }
	/**
	 * @brief get the framebuffers
	 * 
	 * @return const VulkanFramebuffers&
	*/
	const VulkanFramebuffers& framebuffers() const noexcept { return _framebuffers; }
	/**
	 * @brief get the pipeline
	 * 
	 * @return const VulkanGraphicsPipeline&
	*/
	const VulkanGraphicsPipeline& pipeline() const noexcept { return _pipeline; }

private:
	VulkanFramebuffers _framebuffers;
	VulkanGraphicsPipeline _pipeline;

	CallQueue _draw_queue;

	bool _drawing = true;

	Context* context;

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
	 */
	void begin_render_pass() const noexcept;
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
};

}
