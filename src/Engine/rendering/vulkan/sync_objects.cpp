#include <rendering/vulkan/sync_objects.h>

namespace lyra {

// syncronisation objects
VulkanSyncObjects::VulkanSyncObjects() {}

void VulkanSyncObjects::destroy() noexcept {
	for (int i = 0; i <= var.imageAvailableSemaphores.size(); i++){
		vkDestroySemaphore(device->get().device, var.renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(device->get().device, var.imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(device->get().device, var.inFlightFences[i], nullptr);
	}

	delete device;

	LOG_INFO("Succesfully destroyed Vulkan synchronisation objects!")
}

void VulkanSyncObjects::create(VulkanDevice device, const VulkanSwapchain swapchain) {
	LOG_INFO("Creating Vulkan synchronisation objects...")

	this->device = new VulkanDevice(device);

	var.imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	var.renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	var.inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
	VkFenceCreateInfo fenceInfo {
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		nullptr,
		VK_FENCE_CREATE_SIGNALED_BIT
	};

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if(vkCreateSemaphore(device.get().device, &semaphoreInfo, nullptr, &var.imageAvailableSemaphores[i]) != VK_SUCCESS
			|| vkCreateSemaphore(device.get().device, &semaphoreInfo, nullptr, &var.renderFinishedSemaphores[i]) != VK_SUCCESS
			|| vkCreateFence(device.get().device, &fenceInfo, nullptr, &var.inFlightFences[i]) != VK_SUCCESS)
		LOG_EXEPTION("Failed to create Vulkan Synchronisation Objects")
	}

	LOG_INFO("Succesfully created Vulkan synchronisation objects at ", GET_ADDRESS(this), "!", END_L)
}

void VulkanSyncObjects::wait(const uint32 fenceIndex) {
	if(vkWaitForFences(device->get().device, 1, &var.inFlightFences[fenceIndex], VK_TRUE, UINT64_MAX) != VK_SUCCESS) LOG_EXEPTION("Failed to wait for Vulkan fences to finish!")
}

void VulkanSyncObjects::reset(const uint32 fenceIndex) {
	if(vkResetFences(device->get().device, 1, &var.inFlightFences[fenceIndex]) != VK_SUCCESS) LOG_EXEPTION("Failed to reset Vulkan fences!")
}

VulkanSyncObjects::Variables VulkanSyncObjects::get() const noexcept {
	return var;
}

}
