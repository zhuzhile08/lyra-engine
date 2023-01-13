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

#include <vector>
#include <vulkan/vulkan.h>

#include <core/queue.h>

namespace lyra {

/**
 * @brief a vulkan renderer with some basic features
 */
class Renderer {
private:
	// structure containing frame data
	class Frame {
	public:
		/**
		 * @brief construct the frame
		 * 
		 * @param index index of the frame in the renderer
		 */
		Frame() 
		{
			// create the synchronisation objects
			create_sync_objects();
		}
		/**
		 * @brief destruct the frame
		 */
		~Frame();

		/**
		 * @brief recreate the frame
		 *
		 * @param index index of the frame in the renderer
		 */
		void recreate() {
			// create the synchronisation objects
			create_sync_objects();
		}

		/**
		 * @brief get the framebuffers
		 * 
		 * @return VkFramebuffer&
		 */
		NODISCARD const VkFramebuffer& framebuffer() const noexcept { return m_framebuffer; }
		/** 
		 * @brief get the image available semaphores
		 *
		 * @return const VkSemaphor&
		*/
		NODISCARD const VkSemaphore& imageAvailableSemaphores() const noexcept { return m_imageAvailableSemaphore; }
		/**
		 * @brief get the render finished semaphores
		 *
		 * @return const VkSemaphore&
		*/
		NODISCARD const VkSemaphore& renderFinishedSemaphores() const noexcept { return m_renderFinishedSemaphore; }
		/**
		 * @brief get the in flight fences
		 *
		 * @return const VkFence&
		*/
		NODISCARD const VkFence& inFlightFences() const noexcept { return m_inFlightFence; }

	private:
		VkFramebuffer m_framebuffer;

		VkSemaphore m_imageAvailableSemaphore;
		VkSemaphore m_renderFinishedSemaphore;
		VkFence m_inFlightFence;

		/**
		 * @brief create the syncronisation objects
		 */
		void create_sync_objects();
		/**
		 * @brief create the framebuffer
		 */
		void create_framebuffer();

		friend class Renderer;
	};

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
	 * @return const std::vector <Frame>&
	*/
	NODISCARD const std::vector <Frame>& frames() const noexcept { return m_frames; }
	/**
	 * @brief get the framebuffers
	 *
	 * @return const std::vector <VkFramebuffers>&
	*/
	NODISCARD const std::vector <VkFramebuffer>& framebuffers() const noexcept { return m_framebuffers; }

protected:
	VkRenderPass m_renderPass;
	std::vector <Frame> m_frames;
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

	virtual void record_command_buffers() = 0;

	friend class RenderSystem;
};

}
