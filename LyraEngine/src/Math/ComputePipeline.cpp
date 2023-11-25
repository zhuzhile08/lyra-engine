#include <Math/ComputePipeline.h>

#include <Application/Application.h>

namespace lyra {

void ComputePipeline::Builder::build_compute_pipeline(ComputePipeline* const computePipeline) const {
	// creation information
	VkComputePipelineCreateInfo createInfo {
		VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
		nullptr,
		0,
		computePipeline->m_shaders[0].stageCreateInfo(),
		computePipeline->m_layout,
		VK_NULL_HANDLE,
		0
	};

	// create the compute pipeline
	computePipeline->m_pipeline = vulkan::vk::ComputePipeline(Application::renderer.device.device(), VK_NULL_HANDLE, createInfo, { });
}

ComputePipeline::ComputePipeline(const ComputePipeline::Builder& builder) {
	// define what type of pipeline this is
	m_bindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;

	// crate shaders
	builder.build_pipeline_base(this);
	builder.build_compute_pipeline(this);
}

} // namespace lyra
