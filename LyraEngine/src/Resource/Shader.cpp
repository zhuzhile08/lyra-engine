#include <Resource/Shader.h>

#include <Resource/LoadFile.h>
#include <Application/Application.h>
#include <Graphics/VulkanImpl/Device.h>

namespace lyra {

namespace vulkan {

Shader::Shader(std::pair<std::string_view, uint32> pathAndFlags, std::string_view entry)
	 : m_entry(entry), m_type(static_cast<Type>((pathAndFlags.second << 16) / 10000)), m_flags(static_cast<Flags>(pathAndFlags.second / 10000)) { // @todo this is probably not going to work first try
	// load the shader
	std::vector<char> shaderSrc; util::load_file(pathAndFlags.first, util::OpenMode::MODE_BINARY, shaderSrc);

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

