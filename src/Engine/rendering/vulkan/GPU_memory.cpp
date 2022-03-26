#include <rendering/vulkan/GPU_Memory.h>

namespace lyra {

VulkanGPUMemory::VulkanGPUMemory() { }

const VmaAllocationCreateInfo VulkanGPUMemory::alloc_create_info(VmaMemoryUsage usage, VkMemoryPropertyFlags requiredFlags) const noexcept {
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
