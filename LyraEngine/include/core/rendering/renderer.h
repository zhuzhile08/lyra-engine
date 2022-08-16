/*************************
 * @file render_system.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief a rendering system around the Vulkan API with basic features
 *
 * @date 2022-02-04
 *
 * @copyright Copyright (c) 2022 Zhile Zhu
 *************************/

#pragma once

#include <core/decl.h>
#include <core/queue_types.h>

#include <vector>
#include <memory>

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
	 * @brief get the render pass
	 *
	 * @return const VkRenderPass&
	*/
	NODISCARD const VkRenderPass& renderPass() const noexcept { return _renderPass; }
	/**
	 * @brief get the framebuffers
	 *
	 * @return const std::vector <VkFramebuffer>&
	*/
	NODISCARD const std::vector <VkFramebuffer>& framebuffers() const noexcept { return _framebuffers; }

protected:
	VkRenderPass _renderPass = VK_NULL_HANDLE;
	std::vector <VkFramebuffer> _framebuffers;

	CallQueue _updateQueue;

	/**
	 * @brief recreate the renderer
	 */
	void recreate();
	/**
	 * @brief create the render pass
	 */
	void create_render_pass();
	/**
	 * @brief create the framebuffers
	 */
	void create_framebuffers();

	/**
	 * @brief begin the render pass
	 */
	void begin_renderpass() const;
	/**
	 * @brief end the render pass
	 */
	void end_renderpass() const;

	virtual void record_command_buffers() const = 0;

	friend class RenderSystem;
};

}
