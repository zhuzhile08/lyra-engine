#include <res/vulkan_shader.h>

namespace lyra {

VulkanShader::VulkanShader() { }

void VulkanShader::destroy() {
	vkDestroyShaderModule(device->get().device, var.module, nullptr);
}

void VulkanShader::create(VulkanDevice device, const std::string path, str entry, const std::string name, VkShaderStageFlagBits stageFlags) {
	this->device = &device;

	var.entry = entry;
	var.name = name;

	// load the shader
	auto shaderSrc = read_binary(path);

	// create the shader
	VkShaderModuleCreateInfo createInfo {
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		nullptr,
		0,
		shaderSrc.size(),
		reinterpret_cast<const uint32_t*>(shaderSrc.data())
	};

	if(vkCreateShaderModule(device.get().device, &createInfo, nullptr, &var.module) != VK_SUCCESS) LOG_EXEPTION("Failed to create a Vulkan shader module")

	var.stage = {
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		nullptr,
		0,
		stageFlags,
		var.module,
		entry,
		nullptr
	};
}

VulkanShader::Variables VulkanShader::get() const {
	return var;
}

} // namespace lyra