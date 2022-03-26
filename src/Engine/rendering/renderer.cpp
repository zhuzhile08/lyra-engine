#include <rendering/renderer.h>

namespace lyra {

Renderer::Renderer() { ("Renderer", nullptr); }

void Renderer::destroy() noexcept {
	destroy_swapchain();

	_descriptorPool.destroy();
	_descriptorSetLayout.destroy();
	_syncObjects.destroy();
	_commandPool.destroy();
	_device.destroy();
	_instance.destroy();

	delete window;
}

void Renderer::create(Window window) { 
	this->window = new Window(window);

	_instance.create(window);
	_device.create(_instance);
	_commandPool.create(_device);
	_swapchain.create(_device, _instance, window);

	VulkanDescriptorSetLayout::Builder  layoutBuilder;
	layoutBuilder.add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1);
	layoutBuilder.add_binding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_VERTEX_BIT, 1);
	_descriptorSetLayout.create(_device, layoutBuilder);
	
	VulkanDescriptorPool::Builder       poolBuilder;
	poolBuilder.set_max_sets(50);   // just some random number
	poolBuilder.add_pool_sizes(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_FRAMES_IN_FLIGHT);
	poolBuilder.add_pool_sizes(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_FRAMES_IN_FLIGHT);
	_descriptorPool.create(_device, poolBuilder);

	_syncObjects.create(_device, _swapchain);
}

void Renderer::destroy_swapchain() noexcept {
	_swapchain.destroy();
}

void Renderer::recreate_swapchain() {
	VulkanSwapchain oldSwapchain = std::move(_swapchain);

	destroy_swapchain();

	_swapchain.create(oldSwapchain);

	oldSwapchain.destroy();
}

void Renderer::update() {
	while (_running) {
		draw();
	}
	
	_device.wait();
}

void Renderer::draw() {
	_syncObjects.wait(_currentFrame);

	VkResult result = vkAcquireNextImageKHR(_device.device(), _swapchain.swapchain(), UINT64_MAX, _syncObjects.imageAvailableSemaphores()[_currentFrame], VK_NULL_HANDLE, &_imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreate_swapchain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) LOG_EXEPTION("Failed to get the next Vulkan image layer to blit on!");

	_syncObjects.reset(_currentFrame);

	_renderQueue.flush();

	_submitQueue.flush();

	present_queue(_imageIndex);

	update_frame_count();
}

void Renderer::submit_device_queue(
	const VulkanDevice::VulkanQueueFamily   queue,
	const VulkanCommandBuffer               commandBuffer
) const {
	// queue submission info
	VkSubmitInfo submitInfo = {
		VK_STRUCTURE_TYPE_SUBMIT_INFO,
		nullptr,
		0,
		nullptr,
		nullptr,
		1,
		commandBuffer.get_ptr(),
		0,
		nullptr
	};

	// submit the queue
	if (vkQueueSubmit(queue.queue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) LOG_EXEPTION("Failed to submit Vulkan queue!");
}

void Renderer::submit_device_queue(const VulkanDevice::VulkanQueueFamily queue, const VulkanCommandBuffer commandBuffer, const VkPipelineStageFlags stageFlags) const {
	VkSemaphore waitSemaphores[] = { _syncObjects.imageAvailableSemaphores()[_currentFrame]};
	VkSemaphore signalSemaphores[] = { _syncObjects.renderFinishedSemaphores()[_currentFrame]};

	VkSubmitInfo submitInfo = {
	   VK_STRUCTURE_TYPE_SUBMIT_INFO,
	   nullptr,
	   1,
	   waitSemaphores,
	   &stageFlags,
	   1,
	   commandBuffer.get_ptr(),
	   1,
	   signalSemaphores
	};

	// submit the queue
	if (vkQueueSubmit(queue.queue, 1, &submitInfo, _syncObjects.inFlightFences()[_currentFrame]) != VK_SUCCESS) LOG_EXEPTION("Failed to submit Vulkan queue!");
}

void Renderer::present_queue(const uint32 imageIndex) {
	VkSwapchainKHR swapchains[] = { _swapchain.swapchain() };
	VkSemaphore signalSemaphores[] = { _syncObjects.renderFinishedSemaphores()[_currentFrame]};

	VkPresentInfoKHR presentInfo = {
		VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		nullptr,
		1,
		signalSemaphores,
		1,
		swapchains,
		&imageIndex
	};

	VkResult result = vkQueuePresentKHR(_device.presentQueue().queue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		recreate_swapchain();
		LOG_EXEPTION("Failed to get the next Vulkan image layer to blit on!");
	}
	else if (result != VK_SUCCESS) {
		LOG_EXEPTION("Failed to present swapchain image!");
	}
}

void Renderer::wait_device_queue(const VulkanDevice::VulkanQueueFamily queue) const {
	vkQueueWaitIdle(queue.queue);
}

void Renderer::update_frame_count() noexcept {
	_currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

} // namespace lyra
