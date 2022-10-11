#include <rendering/vulkan/GPU_memory.h>

#include <core/logger.h>

#include <core/application.h>
#include <rendering/vulkan/devices.h>

namespace lyra {

namespace vulkan {

GPUMemory::~GPUMemory() {
	Application::renderSystem()->device()->freeMemory(m_memory);

	Logger::log_debug(Logger::tab(), "Successfully destroyed VMA Memory!");
}

const VmaAllocationCreateInfo GPUMemory::get_alloc_create_info(const VmaMemoryUsage usage, const VkMemoryPropertyFlags requiredFlags) noexcept {
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

} // namespace vulkan

} // namespace lyra
