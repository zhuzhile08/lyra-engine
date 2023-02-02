/*************************
 * @file render_system.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief a rendering system around the Vulkan API with basic features
 *
 * @date 2022-02-04
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <lyra.h>

#include <core/settings.h>

#include <core/dynarray.h>
#include <vulkan/vulkan.h>

#include <core/queue.h>

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
	 * @return constexpr const VkRenderPass&
	*/
	NODISCARD constexpr const VkRenderPass& renderPass() const noexcept { return m_renderPass; }
	/**
	 * @brief get the framebuffers
	 *
	 * @return const Dynarray <VkFramebuffers, Settings::RenderConfig::maxSwapchainImages>&
	*/
	NODISCARD const Dynarray <VkFramebuffer, Settings::RenderConfig::maxSwapchainImages>& framebuffers() const noexcept { return m_framebuffers; }

protected:
	VkRenderPass m_renderPass;
	Dynarray <VkFramebuffer, Settings::RenderConfig::maxSwapchainImages> m_framebuffers;

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

	virtual void record_command_buffers() = 0;

	friend class RenderSystem;
};

}
