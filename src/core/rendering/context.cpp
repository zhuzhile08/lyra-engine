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
	VkResult result = vkAcquireNextImageKHR(_device.device(), _swapchain.swapchain(), UINT64_MAX, _syncObjects.imageAvailableSemaphores()[_currentFrame], VK_NULL_HANDLE, &_imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreate_swapchain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) Logger::log_exception("Failed to get the next Vulkan image layer to blit on!");

	// begin recording the command buffer
	_commandBuffers[currentFrame()].begin(0);

	// call the draw calls
	_renderQueue.flush();

	// end recording the command buffer
	_commandBuffers[currentFrame()].end();

	// signal the synchronisation objects to wait until drawing is finished
	submit_device_queue(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
	// executing all recorded commands and render the image onto the window
	present_device_queue();

	// update the frame
	update_frame_count();
}

void Context::submit_device_queue(const VkPipelineStageFlags stageFlags) const {
	VkSemaphore waitSemaphores[] = { _syncObjects.imageAvailableSemaphores()[_currentFrame] };
	VkSemaphore signalSemaphores[] = { _syncObjects.renderFinishedSemaphores()[_currentFrame] };

	VkSubmitInfo submitInfo = {
	   VK_STRUCTURE_TYPE_SUBMIT_INFO,
	   nullptr,
	   1,
	   waitSemaphores,
	   &stageFlags,
	   1,
	   _commandBuffers.at(_currentFrame).get_ptr(),
	   1,
	   signalSemaphores
	};

	// submit the queue
	if (vkQueueSubmit(_device.presentQueue().queue, 1, &submitInfo, _syncObjects.inFlightFences()[_currentFrame]) != VK_SUCCESS) Logger::log_exception("Failed to submit Vulkan queue!");
}

void Context::present_device_queue() {
	VkSwapchainKHR swapchains[] = { _swapchain.swapchain() };
	VkSemaphore signalSemaphores[] = { _syncObjects.renderFinishedSemaphores().at(_currentFrame)};

	VkPresentInfoKHR presentInfo = {
		VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		nullptr,
		1,
		signalSemaphores,
		1,
		swapchains,
		&_imageIndex
	};

	VkResult result = vkQueuePresentKHR(_device.presentQueue().queue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		recreate_swapchain();
	}
	else if (result != VK_SUCCESS) {
		Logger::log_exception("Failed to present swapchain image!");
	}
}

} // namespace lyra
