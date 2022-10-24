#include <rendering/vulkan/GPU_memory.h>

#include <rendering/vulkan/devices.h>

namespace lyra {

namespace vulkan {

GPUMemory::~GPUMemory() {
	Application::renderSystem()->device()->freeMemory(m_memory);

	Logger::log_debug(Logger::tab(), "Successfully destroyed VMA Memory!");
}

} // namespace vulkan

} // namespace lyra
