#include <core/rendering/context.h>

namespace lyra {

Context::Context() { }

void Context::create(Window* const window) {
	Logger::log_info("Creating context for application...");

	this->window = window;

	_instance.create(window);
	_device.create(&_instance);
	_commandPool.create(&_device);
	_commandBuffers.create(&_device, &_commandPool);
	_syncObjects.create(&_device);
	_swapchain.create(&_device, &_instance, &_commandBuffers, window);

	Logger::log_info("Successfully created context for the application at: ", get_address(this), "!", Logger::end_l());
}

void Context::draw() {
	// wait for the already recorded stuff to finish executing
	_syncObjects.wait(_currentFrame); 
	wait_device_queue(_device.presentQueue());
	
	// get the next image to render on
	if (vkAcquireNextImageKHR(_device.device(), _swapchain.swapchain(), UINT64_MAX, _syncObjects.imageAvailableSemaphores()[_currentFrame], VK_NULL_HANDLE, &_imageIndex) == VK_ERROR_OUT_OF_DATE_KHR) {
		_swapchain.recreate();
		_recreateQueue.flush();
		return;
	}

	// reset the semaphores and fences
	_syncObjects.reset(_currentFrame);
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
	_renderQueue.flush();

	// end recording the command buffer
	_commandBuffers.end(_currentCommandBuffer);

	// signal the synchronization objects to wait until drawing is finished
	submit_device_queue(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
	// executing all recorded commands and render the image onto the window
	present_device_queue();

	// update the frame
	update_frame_count();
}

void Context::submit_device_queue(const VkPipelineStageFlags stageFlags) const {
	VkSubmitInfo submitInfo {
	   VK_STRUCTURE_TYPE_SUBMIT_INFO,
	   nullptr,
	   1,
	   &_syncObjects.imageAvailableSemaphores().at(_currentFrame),
	   &stageFlags,
	   1,
	   &_commandBuffers.commandBuffer(_currentCommandBuffer)->commandBuffer,
	   1,
	   &_syncObjects.renderFinishedSemaphores().at(_currentFrame)
	};

	// submit the queue
	vkQueueSubmit(_device.presentQueue().queue, 1, &submitInfo, _syncObjects.inFlightFences().at(_currentFrame));
}

void Context::present_device_queue() {
	VkPresentInfoKHR presentInfo {
		VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		nullptr,
		1,
		&_syncObjects.renderFinishedSemaphores().at(_currentFrame),
		1,
		&_swapchain.swapchain(),
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

		_swapchain.recreate();
		_recreateQueue.flush();
	}
}

} // namespace lyra
