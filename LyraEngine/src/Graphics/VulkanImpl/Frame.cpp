#include <Graphics/VulkanImpl/Frame.h>

#include <Application/Application.h>

namespace lyra {

namespace vulkan {

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
	m_imageAvailableSemaphore = vk::Semaphore(Application::renderSystem.device.device(), semaphoreInfo);
	m_renderFinishedSemaphore = vk::Semaphore(Application::renderSystem.device.device(), semaphoreInfo);

	// fence create info
	VkFenceCreateInfo fenceInfo{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		nullptr,
		VK_FENCE_CREATE_SIGNALED_BIT
	};

	// create the fence
	m_inFlightFence = vk::Fence(Application::renderSystem.device.device(), fenceInfo);
}

void Frame::recreate() {
	this->~Frame();
	create_sync_objects();
}

} // namespace vulkan

} // namespace lyra
