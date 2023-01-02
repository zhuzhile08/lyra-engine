#include <rendering/vulkan/vulkan_shader.h>

#include <res/loaders/load_file.h>
#include <core/application.h>
#include <rendering/vulkan/devices.h>

namespace lyra {

namespace vulkan {

Shader::Shader(const char* path, const char* entry, const Type& type) : m_entry(entry), m_type(type) {
	// load the shader
	std::vector<char> shaderSrc; util::load_file(path, util::OpenMode::MODE_BINARY, shaderSrc);

	// create the shader
	VkShaderModuleCreateInfo createInfo{
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		nullptr,
		0,
		shaderSrc.size(),
		reinterpret_cast<const uint32*>(shaderSrc.data())
	};

	vassert(vkCreateShaderModule(Application::renderSystem.device.device(), &createInfo, nullptr, &m_module), "create a Vulkan shader module");
}

Shader::~Shader() {
	vkDestroyShaderModule(Application::renderSystem.device.device(), m_module, nullptr);
}

} // namespace vulkan

} // namespace lyra

