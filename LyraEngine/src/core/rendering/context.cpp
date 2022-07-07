#include <core/rendering/context.h>

namespace lyra {

Context::Context() { }

void Context::create(const Window* const window) {
	Logger::log_info("Creating context for application...");

	this->window = window;

	_instance.create(window);
	_device.create(&_instance);
	_commandPool.create(&_device);
	_commandBuffers.resize(Settings::Rendering::maxFramesInFlight);
	for (auto& cmdBuff : _commandBuffers) cmdBuff.create(&_device, &_commandPool);
	_syncObjects.create(&_device);
	_swapchain.create(&_device, &_instance, &_commandPool, window);

	Logger::log_info("Successfully created context for the application at: ", get_address(this), "!", Logger::end_l());
}

void Context::draw() {
	// wait for the already recorded stuff to finish executing
	_syncObjects.wait(_currentFrame);
	_syncObjects.reset(_currentFrame);

	// reset command buffer after everything has been executed
	_commandBuffers.at(currentFrame()).reset();

	// get the next image to render on
	if (vkAcquireNextImageKHR(_device.device(), _swapchain.swapchain(), UINT64_MAX, _syncObjects.imageAvailableSemaphores().at(_currentFrame), VK_NULL_HANDLE, &_imageIndex) == VK_ERROR_OUT_OF_DATE_KHR) {
		_swapchain.recreate();
		_recreateQueue.flush();
		return;
	}

	// begin recording the command buffer
	_commandBuffers.at(currentFrame()).begin(0);

	// call the draw calls
	_renderQueue.flush();

	// end recording the command buffer
	_commandBuffers.at(currentFrame()).end();

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
	   _commandBuffers.at(_currentFrame).get_ptr(),
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
			SDL_WaitEvent(const_cast<SDL_Event*>(&window->event()));
		}

		_swapchain.recreate();
		_recreateQueue.flush();
	}
}

} // namespace lyra
