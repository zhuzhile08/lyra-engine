#include <Graphics/RenderSystem.h>

#include <Common/Settings.h>

#include <Input/Input.h>

#include <Graphics/SDLWindow.h>
#include <Graphics/Renderer.h>

namespace lyra {

void RenderSystem::add_renderer(Renderer* const renderer) {
	m_renderers.push_back(renderer);
}

void RenderSystem::wait_device_queue(const vulkan::vk::Queue& queue) const {
	vassert(vkQueueWaitIdle(queue), "wait for device queue");
}

void RenderSystem::draw() {
	// wait for the already recorded stuff to finish executing
	frames[m_currentFrame].wait(); 
	
	// get the next image to render on
	if (vkAcquireNextImageKHR(device.device(), vulkanWindow.swapchain(), UINT64_MAX, frames[m_currentFrame].imageAvailableSemaphores(), VK_NULL_HANDLE, &m_imageIndex) == VK_ERROR_OUT_OF_DATE_KHR) {
		vulkanWindow.recreate();
		for (auto& renderer : m_renderers) renderer->recreate();
		return;
	}

	// reset the semaphores and fences
	frames[m_currentFrame].reset();

	// begin recording the command buffer
	frames[m_currentFrame].commandBuffer().begin();

	// call the draw calls
	for (auto& renderer : m_renderers) renderer->record_command_buffers();

	// end recording the command buffer
	frames[m_currentFrame].commandBuffer().end();

	// signal the synchronization objects to wait until drawing is finished
	submit_device_queue(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
	// executing all recorded commands and render the image onto the window
	present_device_queue();

	// update the frame
	update_frame_count();
}

void RenderSystem::submit_device_queue(const VkPipelineStageFlags& stageFlags) const {
	VkSubmitInfo submitInfo {
	   VK_STRUCTURE_TYPE_SUBMIT_INFO,
	   nullptr,
	   1,
	   &frames[m_currentFrame].imageAvailableSemaphores().get(),
	   &stageFlags,
	   1,
	   &frames[m_currentFrame].commandBuffer().commandBuffer(),
	   1,
	   &frames[m_currentFrame].renderFinishedSemaphores().get()
	};

	// submit the queue
	vkQueueSubmit(device.presentQueue(), 1, &submitInfo, frames[m_currentFrame].inFlightFences());
}

void RenderSystem::present_device_queue() {
	VkPresentInfoKHR presentInfo {
		VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		nullptr,
		1,
		&frames[m_currentFrame].renderFinishedSemaphores().get(),
		1,
		&vulkanWindow.swapchain().get(),
		&m_imageIndex
	};

	VkResult result = vkQueuePresentKHR(device.presentQueue(), &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window->changed()) {
		// check if window was minimized, if true, then loop until otherwise
		SDL_WindowFlags flags = static_cast<SDL_WindowFlags>(SDL_GetWindowFlags(window->get()));
		while ((flags & SDL_WINDOW_MINIMIZED) == SDL_WINDOW_MINIMIZED) {
			flags = static_cast<SDL_WindowFlags>(SDL_GetWindowFlags(window->get()));
			input::InputManager::wait_events();
		}

		vulkanWindow.recreate();
		for (auto& renderer : m_renderers) renderer->recreate();
	} else {
		vassert(result, "present device queue");
	}
}

void RenderSystem::update_frame_count() noexcept {
	m_pastFrame = m_currentFrame;
	m_currentFrame = (m_currentFrame + 1) % Settings::RenderConfig::maxFramesInFlight;
}

} // namespace lyra