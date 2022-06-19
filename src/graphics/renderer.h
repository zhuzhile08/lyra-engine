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
#include <graphics/camera.h>

#include <vector>

#include <noud.h>
#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief a vulkan renderer with some basic features
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
	 */
	void bind() noexcept {
		Application::context()->add_to_update_queue([&]() { _updateQueue.flush(); _camera.draw(); });
		Application::context()->add_to_render_queue([&]() { record_command_buffers(); });
	}

	/**
	 * @brief add a function to the draw queue
	 * 
	 * @param function functio to add
	*/
	void add_to_draw_queue(std::function<void()>&& function) {
		_drawQueue.add(std::move(function));
	}

	/**
	 * @brief add a function to the draw queue
	 *
	 * @param function functio to add
	*/
	void add_to_update_queue(std::function<void()>&& function) {
		_updateQueue.add(std::move(function));
	}

	/**
	 * @brief get the draw queue
	 * 
	 * @return lyra::CallQueue* const
	*/
	[[nodiscard]] const CallQueue* const drawQueue() const noexcept { return &_drawQueue; }
	/**
	 * @brief get the framebuffers
	 * 
	 * @return const lyra::VulkanFramebuffers* const
	*/
	[[nodiscard]] const VulkanFramebuffers* const framebuffers() const noexcept { return &_framebuffers; }
	
	/**
	 * @brief get the camera
	 * 
	 * @return Camera* const
	*/
	[[nodiscard]] Camera* const camera() noexcept { return &_camera; }

private:
	VulkanFramebuffers _framebuffers;
	Camera _camera;

	CallQueue _drawQueue;
	CallQueue _updateQueue;

	bool _drawing = true;

	/**
	 * @brief record all the commands
	 */
	void record_command_buffers() const;
};

}
