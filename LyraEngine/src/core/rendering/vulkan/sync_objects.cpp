#include <core/rendering/vulkan/sync_objects.h>

namespace lyra {

// syncronisation objects
VulkanSyncObjects::VulkanSyncObjects() {}

VulkanSyncObjects::~VulkanSyncObjects() {
	for (int i = 0; i < _imageAvailableSemaphores.size(); i++) {
		vkDestroySemaphore(device->device(), _renderFinishedSemaphores.at(i), nullptr);
		vkDestroySemaphore(device->device(), _imageAvailableSemaphores.at(i), nullptr);
		vkDestroyFence(device->device(), _inFlightFences.at(i), nullptr);
	}

	Logger::log_info("Successfully destroyed Vulkan synchronization objects!");
}

void VulkanSyncObjects::destroy() noexcept {
	this->~VulkanSyncObjects();
}

void VulkanSyncObjects::create(const VulkanDevice* const  device) {
	Logger::log_info("Creating Vulkan synchronization objects...");

	this->device = device;

	_imageAvailableSemaphores.resize(Settings::Rendering::maxFramesInFlight);
	_renderFinishedSemaphores.resize(Settings::Rendering::maxFramesInFlight);
	_inFlightFences.resize(Settings::Rendering::maxFramesInFlight);

	VkSemaphoreCreateInfo semaphoreInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	VkFenceCreateInfo fenceInfo{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		nullptr,
		VK_FENCE_CREATE_SIGNALED_BIT
	};

	for (int i = 0; i < Settings::Rendering::maxFramesInFlight; i++) {
		lassert(vkCreateSemaphore(device->device(), &semaphoreInfo, nullptr, &_imageAvailableSemaphores.at(i)) == VK_SUCCESS
			|| vkCreateSemaphore(device->device(), &semaphoreInfo, nullptr, &_renderFinishedSemaphores.at(i)) == VK_SUCCESS
			|| vkCreateFence(device->device(), &fenceInfo, nullptr, &_inFlightFences.at(i)) == VK_SUCCESS,
			"Failed to create Vulkan Synchronization Objects");
	}

	Logger::log_info("Successfully created Vulkan synchronization objects at ", get_address(this), "!", Logger::end_l());
}

void VulkanSyncObjects::wait(const uint32 fenceIndex) const {
	lassert(vkWaitForFences(device->device(), 1, &_inFlightFences.at(fenceIndex), VK_TRUE, UINT64_MAX) == VK_SUCCESS, "Failed to wait for Vulkan fences to finish!");
}

void VulkanSyncObjects::reset(const uint32 fenceIndex) const {
	lassert(vkResetFences(device->device(), 1, &_inFlightFences.at(fenceIndex)) == VK_SUCCESS, "Failed to reset Vulkan fences!");
}

}