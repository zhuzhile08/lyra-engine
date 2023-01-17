#include <rendering/vulkan/frame.h>

#include <core/application.h>

namespace lyra {

namespace vulkan {

Frame::~Frame() {
	vkDestroySemaphore(Application::renderSystem.device.device(), m_renderFinishedSemaphore, nullptr);
	vkDestroySemaphore(Application::renderSystem.device.device(), m_imageAvailableSemaphore, nullptr);
	vkDestroyFence(Application::renderSystem.device.device(), m_inFlightFence, nullptr);
}

void Frame::wait() const {
	vassert(Application::renderSystem.device.waitForFence(m_inFlightFence, VK_TRUE, UINT64_MAX), "wait for Vulkan fences to finish");
}

void Frame::reset() const {
	vassert(Application::renderSystem.device.resetFence(m_inFlightFence), "reset Vulkan fences");
	m_commandBuffer.reset();
}

void Frame::create_sync_objects() {
	// semaphore create info
	VkSemaphoreCreateInfo semaphoreInfo{
		VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
	};

	// create both semaphores
	vassert(vkCreateSemaphore(Application::renderSystem.device.device(), &semaphoreInfo, nullptr, &m_imageAvailableSemaphore),
		"create Vulkan Synchronization Objects");
	vassert(vkCreateSemaphore(Application::renderSystem.device.device(), &semaphoreInfo, nullptr, &m_renderFinishedSemaphore),
		"create Vulkan Synchronization Objects");

	// fence create info
	VkFenceCreateInfo fenceInfo{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		nullptr,
		VK_FENCE_CREATE_SIGNALED_BIT
	};

	// create the fence
	vassert(vkCreateFence(Application::renderSystem.device.device(), &fenceInfo, nullptr, &m_inFlightFence),
		"create Vulkan Synchronization Objects");
}

void Frame::recreate() {
	this->~Frame();
	create_sync_objects();
}

} // namespace vulkan

} // namespace lyra
