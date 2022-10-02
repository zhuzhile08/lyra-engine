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

#include <vector>
#include <memory>

#include <vulkan/vulkan.h>

#include <core/decl.h>
#include <core/queue_types.h>

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
	virtual ~Renderer();

	/**
	 * @brief get the render pass
	 *
	 * @return const VkRenderPass&
	*/
	NODISCARD const VkRenderPass& renderPass() const noexcept { return m_renderPass; }
	/**
	 * @brief get the framebuffers
	 *
	 * @return const std::vector <VkFramebuffer>&
	*/
	NODISCARD const std::vector <VkFramebuffer>& framebuffers() const noexcept { return m_framebuffers; }

protected:
	VkRenderPass m_renderPass = VK_NULL_HANDLE;
	std::vector <VkFramebuffer> m_framebuffers;

	CallQueue m_updateQueue;

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
