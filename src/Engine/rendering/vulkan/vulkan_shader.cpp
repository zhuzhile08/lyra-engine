#include <rendering/vulkan/vulkan_shader.h>

namespace lyra {

VulkanShader::VulkanShader() { }

void VulkanShader::destroy() noexcept {
	vkDestroyShaderModule(device->device(), _module, nullptr);

	delete device;
}

void VulkanShader::create(VulkanDevice device, const std::string path, str entry, VkShaderStageFlagBits stageFlags) {
	this->device = new VulkanDevice(device);

	_entry = entry;

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

	if (vkCreateShaderModule(device.device(), &createInfo, nullptr, &_module) != VK_SUCCESS) LOG_EXEPTION("Failed to create a Vulkan shader module")

	_stage = {
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		nullptr,
		0,
		stageFlags,
		_module,
		entry,
		nullptr
	};

	LOG_DEBUG(TAB, "Successfully created Vulkan shader from path: ", path, " at: ", GET_ADDRESS(this), "!")
}

} // namespace lyra