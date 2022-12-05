#include <math/vulkan/compute_pipeline.h>

namespace lyra {

void ComputePipeline::create(const CreateInfo info) {
	m_bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;

	create_shaders({ info.shaderCreationInfo });

	create_descriptor_stuff(info.builder);

	create_pipeline(info);
}

void ComputePipeline::create_pipeline(const CreateInfo info) {
	// add all the shader stage creation information into a vector
	VkPipelineShaderStageCreateInfo shaderStage;
	shaderStage = m_shaders.at(0).get_stage_create_info(); // don't question my architecture. It just works.

	create_layout();

	// creation information
	VkComputePipelineCreateInfo createInfo{
		VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
		nullptr,
		0,
		shaderStage,
		m_layout,
		VK_NULL_HANDLE,
		0
	};

	// create the pipelines
	vkCreateComputePipelines(Application::renderSystem.device.device(), VK_NULL_HANDLE, 1, &createInfo, nullptr, &m_pipeline);
}

} // namespace lyra
