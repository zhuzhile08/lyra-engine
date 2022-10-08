#include <core/rendering/render_system.h>

#include <core/logger.h>

#include <core/settings.h>
#include <core/queue_types.h>

#include <core/rendering/window.h>
#include <core/rendering/renderer.h>

namespace lyra {

RenderSystem::RenderSystem(Window* const window) : window(window), m_currentCommandBuffer(&m_commandBuffers) {
	Logger::log_info("Creating context for application...");

	m_renderers.reserve(4);

	Logger::log_info("Successfully created context for the application at: ", get_address(this), "!", Logger::end_l());
}

RenderSystem::~RenderSystem() {
	m_device.wait();

	Logger::log_info("Successfully destroyed application context!");
}

void RenderSystem::add_renderer(Renderer* const renderer) {
	m_renderers.push_back(renderer);
}

void RenderSystem::update() const {
	for (int i = 0; i < m_renderers.size(); i++) m_renderers.at(i)->m_updateQueue.flush();
}

void RenderSystem::wait_device_queue(const vulkan::Device::QueueFamily queue) const {
	vassert(vkQueueWaitIdle(queue.queue), "wait for device queue");
}

void RenderSystem::draw() {
	// wait for the already recorded stuff to finish executing
	m_vulkanWindow.wait(m_currentFrame); 
	wait_device_queue(m_device.presentQueue());
	
	// get the next image to render on
	if (vkAcquireNextImageKHR(m_device.device(), m_vulkanWindow.swapchain(), UINT64_MAX, m_vulkanWindow.imageAvailableSemaphores()[m_currentFrame], VK_NULL_HANDLE, &m_imageIndex) == VK_ERROR_OUT_OF_DATE_KHR) {
		m_vulkanWindow.recreate();
		for (int i = 0; i < m_renderers.size(); i++) m_renderers.at(i)->recreate();
		return;
	}

	// reset the semaphores and fences
	m_vulkanWindow.reset(m_currentFrame);
	// reset command buffer after everything has been executed
	try {
		m_currentCommandBuffer.reset();
	}
	catch (...) { }

	// get a fresh command buffer
	m_currentCommandBuffer = vulkan::CommandBuffer(&m_commandBuffers);

	// begin recording the command buffer
	m_currentCommandBuffer.begin();

	// call the draw calls
	for (int i = 0; i < m_renderers.size(); i++) m_renderers.at(i)->record_command_buffers();

	// end recording the command buffer
	m_currentCommandBuffer.end();

	// signal the synchronization objects to wait until drawing is finished
	submit_device_queue(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
	// executing all recorded commands and render the image onto the window
	present_device_queue();

	// update the frame
	update_frame_count();
}

void RenderSystem::submit_device_queue(const VkPipelineStageFlags stageFlags) const {
	VkSubmitInfo submitInfo {
	   VK_STRUCTURE_TYPE_SUBMIT_INFO,
	   nullptr,
	   1,
	   &m_vulkanWindow.imageAvailableSemaphores().at(m_currentFrame),
	   &stageFlags,
	   1,
	   m_currentCommandBuffer.m_commandBuffer,
	   1,
	   &m_vulkanWindow.renderFinishedSemaphores().at(m_currentFrame)
	};

	// submit the queue
	vkQueueSubmit(m_device.presentQueue().queue, 1, &submitInfo, m_vulkanWindow.inFlightFences().at(m_currentFrame));
}

void RenderSystem::present_device_queue() {
	VkPresentInfoKHR presentInfo {
		VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		nullptr,
		1,
		&m_vulkanWindow.renderFinishedSemaphores().at(m_currentFrame),
		1,
		&m_vulkanWindow.swapchain(),
		&m_imageIndex
	};

	VkResult result = std::move(vkQueuePresentKHR(m_device.presentQueue().queue, &presentInfo));

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || const_cast<Window*>(window)->changed()) {
		// check if window was minimized, if true, then loop until otherwise
		SDL_WindowFlags flags = std::move(static_cast<SDL_WindowFlags>(SDL_GetWindowFlags(window->get())));
		while ((flags & SDL_WINDOW_MINIMIZED) == SDL_WINDOW_MINIMIZED) {
			flags = std::move(static_cast<SDL_WindowFlags>(SDL_GetWindowFlags(window->get())));
			window->wait_events();
		}

		m_vulkanWindow.recreate();
		for (int i = 0; i < m_renderers.size(); i++) m_renderers.at(i)->recreate();
	}
}

void RenderSystem::update_frame_count() noexcept {
	m_currentFrame = (m_currentFrame + 1) % Settings::Rendering::maxFramesInFlight;
}

} // namespace lyra
