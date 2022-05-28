#include <core/rendering/vulkan/vulkan_shader.h>

namespace lyra {

VulkanShader::VulkanShader() { }

void VulkanShader::create(const VulkanDevice* const device, const std::string path, std::string entry, Type type) {
	Logger::log_info("Loading and creating Vulkan shader...");

	Logger::log_debug(Logger::tab(), "Path: ", path);
	Logger::log_debug(Logger::tab(), "Entry point: ", entry);
	Logger::log_debug(Logger::tab(), "Type of shader(VkShaderStageFlagBits): ", static_cast<int>(type));

	this->device = device;

	_entry = entry;
	_type = type;

	// load the shader
	auto shaderSrc = read_binary(path);

	// create the shader
	VkShaderModuleCreateInfo createInfo{
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		nullptr,
		0,
		shaderSrc.size(),
		reinterpret_cast<const uint32*>(shaderSrc.data())
	};

	if (vkCreateShaderModule(device->device(), &createInfo, nullptr, &_module) != VK_SUCCESS) Logger::log_exception("Failed to create a Vulkan shader module");

	Logger::log_info(Logger::tab(), "Successfully created Vulkan shader from at: ", get_address(this), "!");
}

} // namespace lyra