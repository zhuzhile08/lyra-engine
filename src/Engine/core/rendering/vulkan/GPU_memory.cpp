#include <core/rendering/vulkan/GPU_memory.h>

namespace lyra {

VulkanGPUMemory::VulkanGPUMemory() { }

const VmaAllocationCreateInfo VulkanGPUMemory::get_alloc_create_info(const VmaMemoryUsage usage, const VkMemoryPropertyFlags requiredFlags) const noexcept {
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
