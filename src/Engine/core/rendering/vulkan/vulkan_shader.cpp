#include <core/rendering/vulkan/vulkan_shader.h>

namespace lyra {

VulkanShader::VulkanShader() { }

void VulkanShader::destroy() noexcept {
	vkDestroyShaderModule(device->device(), _module, nullptr);
}

void VulkanShader::create(const VulkanDevice* device, const std::string path, str entry, VkShaderStageFlagBits stageFlags) {
	this->device = device;

	_entry = entry;
	_stageFlags = stageFlags;

	// load the shader
	auto shaderSrc = read_binary(path);

	// create the shader
	VkShaderModuleCreateInfo createInfo{
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		nullptr,
		0,
		shaderSrc.size(),
		reinterpret_cast<const uint32_t*>(shaderSrc.data())
	};

	if (vkCreateShaderModule(device->device(), &createInfo, nullptr, &_module) != VK_SUCCESS) LOG_EXEPTION("Failed to create a Vulkan shader module")

	LOG_DEBUG(TAB, "Successfully created Vulkan shader from path: ", path, " at: ", GET_ADDRESS(this), "!")
}

const VkPipelineShaderStageCreateInfo VulkanShader::get_stage_create_info() const noexcept {
	return {
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		nullptr,
		0,
		_stageFlags,
		_module,
		_entry,
		nullptr
	};
}

} // namespace lyra