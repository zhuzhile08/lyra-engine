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
#include <core/core.h>

#include <vector>

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
	void destroy() noexcept {
		this->~Renderer();
	}

	Renderer operator=(const Renderer&) const noexcept = delete;

	/**
	 * @brief bind the functions for resetting and finish recording the command buffers
	 */
	void bind() noexcept;

	/**
	 * @brief add a function to the draw queue
	 * 
	 * @param function functio to add
	*/
	void add_to_draw_queue(std::function<void()>&& function);

	/**
	 * @brief add a function to the draw queue
	 *
	 * @param function functio to add
	*/
	void add_to_update_queue(std::function<void()>&& function);

	/**
	 * @brief get the render pass
	 *
	 * @return const VkRenderPass&
	*/
	[[nodiscard]] const VkRenderPass& renderPass() const noexcept { return _renderPass; }
	/**
	 * @brief get the framebuffers
	 *
	 * @return const std::vector <VkFramebuffer>&
	*/
	[[nodiscard]] const std::vector <VkFramebuffer>& framebuffers() const noexcept { return _framebuffers; }

protected:
	VkRenderPass _renderPass = VK_NULL_HANDLE;
	std::vector <VkFramebuffer> _framebuffers;

	CallQueue* _drawQueue;
	CallQueue* _updateQueue;

	/**
	 * @brief create the render pass
	 */
	void create_render_pass();
	/**
	 * @brief create the framebuffers
	 */
	void create_framebuffers();

	/**
	 * @brief record all the commands
	 */
	void record_command_buffers() const;
};

}
