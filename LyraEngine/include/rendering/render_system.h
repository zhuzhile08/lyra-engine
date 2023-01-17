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
#include <core/array.h>

#include <vulkan/vulkan.h>

#include <rendering/vulkan/devices.h>
#include <rendering/vulkan/frame.h>
#include <rendering/vulkan/command_buffer.h>
#include <rendering/vulkan/vulkan_window.h>

namespace lyra {

/**
 * @brief a vulkan context containing everything needed for an application
 */
class RenderSystem {
public:
	/**
	 * @brief create the renderer
	 *
	 * @param window the window
	 */
	RenderSystem(Window* const window) : window(window) { }

	RenderSystem(const RenderSystem&) noexcept = delete;
	RenderSystem operator=(const RenderSystem&) const noexcept = delete;

	/**
	 * @brief wait for queue to finish submitting
	 *
	 * @param queue queue to wait for
	 */
	void wait_device_queue(const vulkan::Device::QueueFamily& queue) const;

	/**
	 * @brief take the recorded commands and draw everything
	 */
	void draw();

	/**
	 * @brief get the current frame count
	 * 
	 * @return const uint8
	*/
	NODISCARD const uint8 currentFrame() const noexcept { return m_currentFrame; }
	/**
	 * @brief get the image index
	 * 
	 * @return const uint32
	*/
	NODISCARD const uint32 imageIndex() const noexcept { return m_imageIndex; }

	vulkan::Device device;
	Array<vulkan::CommandPool, 4> commandPools;
	vulkan::Window vulkanWindow;
	Array<vulkan::Frame, 2> frames;

private:
	std::vector<Renderer*> m_renderers;

	uint8 m_currentFrame = 0;
	uint8 m_pastFrame;
	uint32 m_imageIndex;

	Window* window;

	/**
	 * @brief add a renderer to the vector of renderers
	 *
	 * @param renderer renderer
	*/
	void add_renderer(Renderer* const renderer);

	/**
	 * @brief present all the recorded commands
	 */
	void present_device_queue();
	/**
	 * @brief submit the device presentation queue after command recording
	 *
	 * @param stageFlags pipeline shader stage flags
	 */
	void submit_device_queue(const VkPipelineStageFlags& stageFlags) const;
	
	/// @todo
	/**
	 * @brief bind a sparce device queue
	
	void bind_sparce_device_queue(const VkBindSparseInfo* pBindInfo) const;
	 */

	/**
	 * @brief update the frame count
	 */
	void update_frame_count() noexcept;

	friend class Renderer;
	friend class Camera;
};

} // namespace lyra
