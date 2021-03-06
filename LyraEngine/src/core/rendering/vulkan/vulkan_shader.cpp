#include <core/rendering/vulkan/vulkan_shader.h>

#include <res/loaders/load_file.h>
#include <core/rendering/vulkan/devices.h>

namespace lyra {

VulkanShader::~VulkanShader() {
	vkDestroyShaderModule(device->device(), _module, nullptr);

	Logger::log_info("Successfully destroyed loaded Vulkan shader!");
}

void VulkanShader::create(const VulkanDevice* const device, const char* path, const char* entry, Type type) {
	Logger::log_info("Loading and creating Vulkan shader...");

	Logger::log_debug(Logger::tab(), "Path: ", path);
	Logger::log_debug(Logger::tab(), "Entry point: ", entry);
	Logger::log_debug(Logger::tab(), "Type of shader(VkShaderStageFlagBits): ", static_cast<int>(type));

	this->device = device;

	_entry = entry;
	_type = type;

	// load the shader
	std::vector<char> shaderSrc = { }; load_file(path, OpenMode::MODE_START_AT_END | OpenMode::MODE_BINARY, &shaderSrc);

	// create the shader
	VkShaderModuleCreateInfo createInfo{
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		nullptr,
		0,
		shaderSrc.size(),
		reinterpret_cast<const uint32*>(shaderSrc.data())
	};

	lassert(vkCreateShaderModule(device->device(), &createInfo, nullptr, &_module) == VK_SUCCESS, "Failed to create a Vulkan shader module");

	Logger::log_info(Logger::tab(), "Successfully created Vulkan shader from at: ", get_address(this), "!");
}

} // namespace lyra

