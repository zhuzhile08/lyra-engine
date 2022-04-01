#include <core/rendering/vulkan/sync_objects.h>

namespace lyra {

// syncronisation objects
VulkanSyncObjects::VulkanSyncObjects() {}

void VulkanSyncObjects::destroy() noexcept {
	for (int i = 0; i <= _imageAvailableSemaphores.size(); i++) {
		vkDestroySemaphore(device->device(), _renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(device->device(), _imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(device->device(), _inFlightFences[i], nullptr);
	}

	LOG_INFO("Succesfully destroyed Vulkan synchronisation objects!")
}

void VulkanSyncObjects::create(const VulkanDevice* device, const VulkanSwapchain* swapchain) {
	LOG_INFO("Creating Vulkan synchronisation objects...")

	this->device = device;

	_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	VkFenceCreateInfo fenceInfo{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		nullptr,
		VK_FENCE_CREATE_SIGNALED_BIT
	};

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(device->device(), &semaphoreInfo, nullptr, &_imageAvailableSemaphores[i]) != VK_SUCCESS
			|| vkCreateSemaphore(device->device(), &semaphoreInfo, nullptr, &_renderFinishedSemaphores[i]) != VK_SUCCESS
			|| vkCreateFence(device->device(), &fenceInfo, nullptr, &_inFlightFences[i]) != VK_SUCCESS)
			LOG_EXEPTION("Failed to create Vulkan Synchronisation Objects")
	}

	LOG_INFO("Succesfully created Vulkan synchronisation objects at ", GET_ADDRESS(this), "!", END_L)
}

void VulkanSyncObjects::wait(const uint32 fenceIndex) {
	if (vkWaitForFences(device->device(), 1, &_inFlightFences[fenceIndex], VK_TRUE, UINT64_MAX) != VK_SUCCESS) LOG_EXEPTION("Failed to wait for Vulkan fences to finish!")
}

void VulkanSyncObjects::reset(const uint32 fenceIndex) {
	if (vkResetFences(device->device(), 1, &_inFlightFences[fenceIndex]) != VK_SUCCESS) LOG_EXEPTION("Failed to reset Vulkan fences!")
}

}
