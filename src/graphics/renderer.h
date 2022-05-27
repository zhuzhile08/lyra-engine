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

#include <lyra.h>
#include <core/defines.h>
#include <core/logger.h>
#include <core/queue_types.h>
#include <core/rendering/vulkan/command_buffer.h>
#include <core/rendering/vulkan/descriptor.h>
#include <core/rendering/vulkan/framebuffers.h>
#include <core/rendering/vulkan/GPU_buffer.h>
#include <core/rendering/context.h>
#include <graphics/graphics_pipeline.h>

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

	Renderer operator=(const Renderer&) const noexcept = delete;

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
	const CallQueue* draw_queue() const noexcept { return& _draw_queue; }
	/**
	 * @brief get the framebuffers
	 * 
	 * @return const lyra::VulkanFramebuffers*
	*/
	const VulkanFramebuffers* framebuffers() const noexcept { return &_framebuffers; }
	/**
	 * @brief get the pipeline
	 * 
	 * @return const lyra::GraphicsPipeline*
	*/
	const GraphicsPipeline* pipeline() const noexcept { return &_pipeline; }

private:
	VulkanFramebuffers _framebuffers;
	GraphicsPipeline _pipeline;

	CallQueue _draw_queue;

	bool _drawing = true;

	/**
	 * @brief bind a descriptor set
	 *
	 * @param descriptor descriptor
	 * @param pipelineLayout pipeline layout
	 */
	void bind_descriptor(const VulkanDescriptor* descriptor) const noexcept;

	/**
	 * @brief record all the commands
	 */
	void record_command_buffers();

	friend class Mesh;
};

}
