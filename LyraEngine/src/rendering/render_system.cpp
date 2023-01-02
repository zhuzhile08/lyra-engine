#include <rendering/render_system.h>

#include <core/settings.h>
#include <core/queue.h>

#include <input/input.h>

#include <rendering/window.h>
#include <rendering/renderer.h>

namespace lyra {

RenderSystem::~RenderSystem() {
	// device.wait();
}

void RenderSystem::add_renderer(Renderer* const renderer) {
	for (uint32 i = 0; i < m_renderers.size(); i++) m_renderers[i] = renderer;
}

void RenderSystem::wait_device_queue(const vulkan::Device::QueueFamily& queue) const {
	vassert(vkQueueWaitIdle(queue.queue), "wait for device queue");
}

void RenderSystem::draw() {
	// update the renderers
	for (uint32 i = 0; i < m_renderers.size(); i++) m_renderers[i]->m_updateQueue.flush();

	// wait for the already recorded stuff to finish executing
	vulkanWindow.wait(m_currentFrame); 
	
	// get the next image to render on
	if (vkAcquireNextImageKHR(device.device(), vulkanWindow.swapchain(), UINT64_MAX, vulkanWindow.imageAvailableSemaphores()[m_currentFrame], VK_NULL_HANDLE, &m_imageIndex) == VK_ERROR_OUT_OF_DATE_KHR) {
		vulkanWindow.recreate();
		for (uint32 i = 0; i < m_renderers.size(); i++) m_renderers[i]->recreate();
		return;
	}

	// reset the semaphores and fences
	vulkanWindow.reset(m_currentFrame);
	// reset command buffer after everything has been executed
	currentCommandBuffer.reset();

	// get a fresh command buffer
	currentCommandBuffer.find_new_commandBuffer();

	// begin recording the command buffer
	currentCommandBuffer.begin();

	// call the draw calls
	for (uint32 i = 0; i < m_renderers.size(); i++) m_renderers[i]->record_command_buffers();

	// end recording the command buffer
	currentCommandBuffer.end();

	// signal the synchronization objects to wait until drawing is finished
	submit_device_queue(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
	// executing all recorded commands and render the image onto the window
	present_device_queue();

	// update the frame
	update_frame_count();

	wait_device_queue(device.presentQueue());
}

void RenderSystem::submit_device_queue(const VkPipelineStageFlags& stageFlags) const {
	VkSubmitInfo submitInfo {
	   VK_STRUCTURE_TYPE_SUBMIT_INFO,
	   nullptr,
	   1,
	   &vulkanWindow.imageAvailableSemaphores().at(m_currentFrame),
	   &stageFlags,
	   1,
	   currentCommandBuffer.m_commandBuffer,
	   1,
	   &vulkanWindow.renderFinishedSemaphores().at(m_currentFrame)
	};

	// submit the queue
	vkQueueSubmit(device.presentQueue().queue, 1, &submitInfo, vulkanWindow.inFlightFences().at(m_currentFrame));
}

void RenderSystem::present_device_queue() {
	VkPresentInfoKHR presentInfo {
		VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		nullptr,
		1,
		&vulkanWindow.renderFinishedSemaphores()[m_currentFrame],
		1,
		&vulkanWindow.swapchain(),
		&m_imageIndex
	};

	VkResult result = vkQueuePresentKHR(device.presentQueue().queue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window->changed()) {
		// check if window was minimized, if true, then loop until otherwise
		SDL_WindowFlags flags = std::move(static_cast<SDL_WindowFlags>(SDL_GetWindowFlags(window->get())));
		while ((flags & SDL_WINDOW_MINIMIZED) == SDL_WINDOW_MINIMIZED) {
			flags = std::move(static_cast<SDL_WindowFlags>(SDL_GetWindowFlags(window->get())));
			Input::wait_events();
		}

		vulkanWindow.recreate();
		for (uint32 i = 0; i < m_renderers.size(); i++) m_renderers[i]->recreate();
	}
}

void RenderSystem::update_frame_count() noexcept {
	m_currentFrame = (m_currentFrame + 1) % Settings::Rendering::maxFramesInFlight;
}

} // namespace lyra
