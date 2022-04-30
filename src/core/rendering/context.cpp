#include <core/rendering/context.h>

namespace lyra {

Context::Context() { }

Context::~Context() noexcept {
	_device.wait();

	LOG_INFO("Successfully destroyed application context!");
}

void Context::destroy() noexcept {
	this->~Context();
}

void Context::create(const Window* const window) {
	LOG_INFO("Creating context for application...");

	this->window = window;

	_instance.create(window);
	_device.create(&_instance);
	_commandPool.create(&_device);
	_commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	for (auto& cmdBuff : _commandBuffers) cmdBuff.create(&_device, &_commandPool);
	_syncObjects.create(&_device);
	_swapchain.create(&_device, &_instance, &_commandPool, window);

	VulkanDescriptorSetLayout::Builder  layoutBuilder;
	layoutBuilder.add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
	layoutBuilder.add_binding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	_descriptorSetLayout.create(&_device, layoutBuilder);

	VulkanDescriptorPool::Builder       poolBuilder;
	poolBuilder.set_max_sets(21);   // 9 + 10 is 21, of course, what else whould it be, 19?
	poolBuilder.add_pool_sizes({ { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_FRAMES_IN_FLIGHT }, { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_FRAMES_IN_FLIGHT } });
	_descriptorPool.create(&_device, poolBuilder);

	LOG_INFO("Successfully created context for the application at: ", GET_ADDRESS(this), "!", END_L);
}

void Context::recreate_swapchain() {
	VkSwapchainKHR oldSwapchain = std::move(_swapchain.swapchain());

	_swapchain.destroy();

	_swapchain.create(&oldSwapchain, &_commandPool);
}

void Context::add_to_render_queue(std::function<void()> &&function) {
	_renderQueue.add(std::move(function));
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
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) LOG_EXEPTION("Failed to get the next Vulkan image layer to blit on!");

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
	if (vkQueueSubmit(_device.presentQueue().queue, 1, &submitInfo, _syncObjects.inFlightFences()[_currentFrame]) != VK_SUCCESS) LOG_EXEPTION("Failed to submit Vulkan queue!");
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
		LOG_EXEPTION("Failed to present swapchain image!");
	}
}

void Context::wait_device_queue(const VulkanDevice::VulkanQueueFamily queue) const {
	if (vkQueueWaitIdle(queue.queue) != VK_SUCCESS) LOG_EXEPTION("Failed to wait for device queue!");
}

void Context::update_frame_count() noexcept {
	_currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

} // namespace lyra
