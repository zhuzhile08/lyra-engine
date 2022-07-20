#include <core/rendering/vulkan/GPU_memory.h>

#include <core/logger.h>

#include <core/rendering/vulkan/devices.h>

namespace lyra {

VulkanGPUMemory::~VulkanGPUMemory() {
	vmaFreeMemory(device->allocator(), _memory);

	Logger::log_debug(Logger::tab(), "Successfully destroyed VMA Memory!");
}

const VmaAllocationCreateInfo VulkanGPUMemory::get_alloc_create_info(const VulkanDevice* const device, const VmaMemoryUsage usage, const VkMemoryPropertyFlags requiredFlags) noexcept {
	this->device = device;

	return {
		0,
		usage,
		requiredFlags,
		0,
		0,
		0,
		nullptr,
		0
	}; // the rest is absolutely useless
}

} // namespace lyra
