/*************************
 * @file RenderSystem.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief a rendering system around the Vulkan API with basic features
 *
 * @date 2022-02-04
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <Lyra/Lyra.h>

#include <Common/Settings.h>

#include <Common/Dynarray.h>
#include <vulkan/vulkan.h>
#include <Common/RAIIContainers.h>

#include <Common/Queue.h>

namespace lyra {

/**
 * @brief a vulkan renderer with some basic features
 */
class Renderer {
public:
	Renderer();
	DEFINE_DEFAULT_MOVE(Renderer)

	/**
	 * @brief get the render pass
	 *
	 * @return constexpr const lyra::vulkan::vk::RenderPass&
	*/
	NODISCARD constexpr const vulkan::vk::RenderPass& renderPass() const noexcept { return m_renderPass; }
	/**
	 * @brief get the framebuffers
	 *
	 * @return const lyra::Dynarray<lyra::vulkan::vk::Framebuffer, lyra::Settings::RenderConfig::maxSwapchainImages>&
	*/
	NODISCARD constexpr const lyra::Dynarray<vulkan::vk::Framebuffer, Settings::RenderConfig::maxSwapchainImages>& framebuffers() const noexcept { return m_framebuffers; }

protected:
	vulkan::vk::RenderPass m_renderPass;
	Dynarray <vulkan::vk::Framebuffer, Settings::RenderConfig::maxSwapchainImages> m_framebuffers;

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