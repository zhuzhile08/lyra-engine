#include <rendering/vulkan/GPU_memory.h>
#include <core/application.h>

namespace lyra {

namespace vulkan {

GPUMemory::~GPUMemory() {
	Application::renderSystem.device.freeMemory(m_memory);
}

} // namespace vulkan

} // namespace lyra
