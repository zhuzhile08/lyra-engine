#include <math/vulkan/compute_pipeline.h>

namespace lyra {

void ComputePipeline::create(const CreateInfo info) {
	Logger::log_info("Creating Vulkan compute pipeline...");

	_bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;

	create_shaders({ info.shaderCreationInfo });

	create_descriptor_stuff(info.layoutBuilder, info.poolBuilder);

	create_pipeline(info);

	Logger::log_info("Successfully created Vulkan pipeline at ", get_address(this), "!", Logger::end_l());
}

void ComputePipeline::create_pipeline(const CreateInfo info) {
	// add all the shader stage creation information into a vector
	VkPipelineShaderStageCreateInfo shaderStage;
	shaderStage = _shaders.at(0).get_stage_create_info(); // don't question my architecture. It just works.

	create_layout();

	// creation information
	VkComputePipelineCreateInfo createInfo{
		VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
		nullptr,
		0,
		shaderStage,
		_layout,
		VK_NULL_HANDLE,
		0
	};

	// create the pipelines
	vkCreateComputePipelines(Application::context()->device()->device(), VK_NULL_HANDLE, 1, &createInfo, nullptr, &_pipeline);
}

} // namespace lyra
