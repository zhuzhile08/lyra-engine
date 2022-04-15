#include <core/rendering/vulkan/GPU_memory.h>

namespace lyra {

VulkanGPUMemory::VulkanGPUMemory() { }

const VmaAllocationCreateInfo* VulkanGPUMemory::alloc_create_info(VmaMemoryUsage usage, VkMemoryPropertyFlags requiredFlags) const noexcept {
	VmaAllocationCreateInfo info = {
		0,
		usage,
		requiredFlags,
		0,
		0,
		0,
		nullptr,
		0
	}; // the rest is absolutely useless

	return &info;
}

} // namespace lyra
