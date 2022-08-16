#include <core/rendering/render_system.h>

#include <core/logger.h>

#include <core/queue_types.h>

#include <core/rendering/window.h>
#include <core/rendering/renderer.h>

namespace lyra {

RenderSystem::RenderSystem(Window* const window) : window(window) {
	Logger::log_info("Creating context for application...");

	_renderers.reserve(4);

	Logger::log_info("Successfully created context for the application at: ", get_address(this), "!", Logger::end_l());
}

RenderSystem::~RenderSystem() {
	_device.wait();

	Logger::log_info("Successfully destroyed application context!");
}

void RenderSystem::add_renderer(Renderer* const renderer) {
	_renderers.push_back(renderer);
}

void RenderSystem::update() const {
	for (int i = 0; i < _renderers.size(); i++) _renderers.at(i)->_updateQueue.flush();
}

void RenderSystem::wait_device_queue(const VulkanDevice::VulkanQueueFamily queue) const {
	lassert(vkQueueWaitIdle(queue.queue) == VK_SUCCESS, "Failed to wait for device queue!");
}

const VkCommandBuffer& RenderSystem::activeCommandBuffer() noexcept { 
	return _commandBuffers.commandBuffer(_currentCommandBuffer)->commandBuffer; 
}

void RenderSystem::draw() {
	// wait for the already recorded stuff to finish executing
	_vulkanWindow.wait(_currentFrame); 
	wait_device_queue(_device.presentQueue());
	
	// get the next image to render on
	if (vkAcquireNextImageKHR(_device.device(), _vulkanWindow.swapchain(), UINT64_MAX, _vulkanWindow.imageAvailableSemaphores()[_currentFrame], VK_NULL_HANDLE, &_imageIndex) == VK_ERROR_OUT_OF_DATE_KHR) {
		_vulkanWindow.recreate();
		for (int i = 0; i < _renderers.size(); i++) _renderers.at(i)->recreate();
		return;
	}

	// reset the semaphores and fences
	_vulkanWindow.reset(_currentFrame);
	// reset command buffer after everything has been executed
	try {
		_commandBuffers.reset(_currentCommandBuffer);
	}
	catch (...) { }

	// get a fresh command buffer
	_currentCommandBuffer = _commandBuffers.get_unused();

	// begin recording the command buffer
	_commandBuffers.begin(_currentCommandBuffer, 0);

	// call the draw calls
	for (int i = 0; i < _renderers.size(); i++) _renderers.at(i)->record_command_buffers();

	// end recording the command buffer
	_commandBuffers.end(_currentCommandBuffer);

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
	   &_vulkanWindow.imageAvailableSemaphores().at(_currentFrame),
	   &stageFlags,
	   1,
	   &_commandBuffers.commandBuffer(_currentCommandBuffer)->commandBuffer,
	   1,
	   &_vulkanWindow.renderFinishedSemaphores().at(_currentFrame)
	};

	// submit the queue
	vkQueueSubmit(_device.presentQueue().queue, 1, &submitInfo, _vulkanWindow.inFlightFences().at(_currentFrame));
}

void RenderSystem::present_device_queue() {
	VkPresentInfoKHR presentInfo {
		VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		nullptr,
		1,
		&_vulkanWindow.renderFinishedSemaphores().at(_currentFrame),
		1,
		&_vulkanWindow.swapchain(),
		&_imageIndex
	};

	VkResult result = std::move(vkQueuePresentKHR(_device.presentQueue().queue, &presentInfo));

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || const_cast<Window*>(window)->changed()) {
		// check if window was minimized, if true, then loop until otherwise
		SDL_WindowFlags flags = std::move(static_cast<SDL_WindowFlags>(SDL_GetWindowFlags(window->get())));
		while ((flags & SDL_WINDOW_MINIMIZED) == SDL_WINDOW_MINIMIZED) {
			flags = std::move(static_cast<SDL_WindowFlags>(SDL_GetWindowFlags(window->get())));
			window->wait_events();
		}

		_vulkanWindow.recreate();
		for (int i = 0; i < _renderers.size(); i++) _renderers.at(i)->recreate();
	}
}

void RenderSystem::update_frame_count() noexcept {
	_currentFrame = (_currentFrame + 1) % Settings::Rendering::maxFramesInFlight;
}

} // namespace lyra
