#include <core/rendering/vulkan/vulkan_pipeline.h>

namespace lyra {

VulkanPipeline::~VulkanPipeline() noexcept {
	vkDestroyPipeline(Application::context()->device()->device(), _pipeline, nullptr);
	vkDestroyPipelineLayout(Application::context()->device()->device(), _layout, nullptr);
	_shaders.clear();

	Logger::log_info("Successfully destroyed Vulkan pipeline!");
}

void VulkanPipeline::create_shaders(std::vector<ShaderCreationInfo> shaderCreationInfos) {
	if (shaderCreationInfos.size() != _shaders.size()) {
		Logger::log_warning("Number of shader creation infos doesn't match up with the numbers of shaders in the pipeline at: ", get_address(this), "!");
	}

	for (int index = 0; index < shaderCreationInfos.size(); index++) {
		_shaders.at(index).create(Application::context()->device(), shaderCreationInfos.at(index).path, shaderCreationInfos.at(index).entry, shaderCreationInfos.at(index).type);
		Logger::log_info("Successfully created Vulkan shader at: ", get_address(&_shaders.at(index)), " with flag: ", shaderCreationInfos.at(index).type, "!");
	}
}

void VulkanPipeline::create_descriptor_stuff(Builder builder) {
	// configure the builders using the custom pipeline builder
	VulkanDescriptorSetLayout::Builder layoutBuilder; // layout
	layoutBuilder.bindings = builder.bindings;

	VulkanDescriptorPool::Builder poolBuilder; // pool
	poolBuilder.maxSets = builder.maxSets;
	poolBuilder.poolFlags = builder.poolFlags;
	poolBuilder.poolSizes = builder.poolSizes;

	// create the descriptor layout and pool
	_descriptorSetLayout.create(layoutBuilder);
	_descriptorPool.create(poolBuilder);
}

} // namespace lyra
