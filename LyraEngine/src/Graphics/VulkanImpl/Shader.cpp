#include <Graphics/VulkanImpl/Shader.h>

#include <Resource/LoadFile.h>
#include <Application/Application.h>
#include <Graphics/VulkanImpl/Device.h>

namespace lyra {

namespace vulkan {

Shader::Shader(std::string_view path, std::string_view entry, const Type& type) : m_entry(entry), m_type(type) {
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

	m_module = vk::ShaderModule(Application::renderSystem.device.device(), createInfo);
}

} // namespace vulkan

} // namespace lyra

