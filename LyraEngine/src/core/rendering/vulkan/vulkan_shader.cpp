#include <core/rendering/vulkan/vulkan_shader.h>

#include <res/loaders/load_file.h>
#include <core/application.h>
#include <core/rendering/vulkan/devices.h>

namespace lyra {

namespace vulkan {

Shader::Shader(const char* path, const char* entry, Type type) : m_entry(entry), m_type(type) {
	Logger::log_info("Loading and creating Vulkan shader...");

	Logger::log_debug(Logger::tab(), "Path: ", path);
	Logger::log_debug(Logger::tab(), "Entry point: ", entry);
	Logger::log_debug(Logger::tab(), "Type of shader(VkShaderStageFlagBits): ", static_cast<int>(type));

	// load the shader
	std::vector<char> shaderSrc = util::load_file<std::vector<char>>(path, util::OpenMode::MODE_START_AT_END | util::OpenMode::MODE_BINARY);

	// create the shader
	VkShaderModuleCreateInfo createInfo{
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		nullptr,
		0,
		shaderSrc.size(),
		reinterpret_cast<const uint32*>(shaderSrc.data())
	};

	lassert(vkCreateShaderModule(Application::renderSystem()->device()->device(), &createInfo, nullptr, &m_module) == VkResult::VK_SUCCESS, "Failed to create a Vulkan shader module");

	Logger::log_info(Logger::tab(), "Successfully created Vulkan shader from at: ", get_address(this), "!");
}

Shader::~Shader() {
	vkDestroyShaderModule(Application::renderSystem()->device()->device(), m_module, nullptr);

	Logger::log_info("Successfully destroyed loaded Vulkan shader!");
}

} // namespace vulkan

} // namespace lyra

