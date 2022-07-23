#include <core/rendering/vulkan/vulkan_pipeline.h>

#include <core/logger.h>

#include <core/rendering/vulkan/devices.h>
#include <core/rendering/vulkan/vulkan_shader.h>
#include <core/rendering/vulkan/command_buffer.h>
#include <core/rendering/vulkan/descriptor.h>

namespace lyra {

// builder
void VulkanPipeline::Builder::add_bindings(std::vector<std::tuple<const uint32, const int, const int, const uint32, const uint32>> newBindings) noexcept {
	for (const auto& [binding, type, shaderType, count, allocCount] : newBindings) {
		bindings.push_back({
			binding,
			static_cast<VkDescriptorType>(type),
			count,
			static_cast<VkShaderStageFlags>(shaderType),
			nullptr
			});
		poolSizes.push_back({
			static_cast<VkDescriptorType>(type),
			allocCount
			});
	}
}

// pipeline
VulkanPipeline::~VulkanPipeline() noexcept {
	delete _descriptorPool;
	delete _descriptorSetLayout;

	vkDestroyPipeline(Application::context()->device()->device(), _pipeline, nullptr);
	vkDestroyPipelineLayout(Application::context()->device()->device(), _layout, nullptr);
	_shaders.clear();

	Logger::log_info("Successfully destroyed Vulkan pipeline!");
}

void VulkanPipeline::create_layout() {
	// create the pipeline layout
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		nullptr,
		0,
		1,
		_descriptorSetLayout->get_ptr(),
		0,    /// @todo push constants
		nullptr
	};

	lassert(vkCreatePipelineLayout(Application::context()->device()->device(), &pipelineLayoutInfo, nullptr, &_layout) == VK_SUCCESS, "Failed to create Vulkan graphics pipeline layout!");
}

void VulkanPipeline::create_shaders(std::vector<ShaderCreationInfo> shaderCreationInfos) {
	if (shaderCreationInfos.size() != _shaders.size()) {
		Logger::log_warning("Number of shader creation infos doesn't match up with the numbers of shaders in the pipeline at: ", get_address(this), "!");
	}

	for (int index = 0; index < shaderCreationInfos.size(); index++) {
		_shaders.at(index).create(Application::context()->device(), shaderCreationInfos.at(index).path, shaderCreationInfos.at(index).entry, 
			static_cast<VulkanShader::Type>(shaderCreationInfos.at(index).type));
		Logger::log_info("Successfully created Vulkan shader at: ", get_address(&_shaders.at(index)), " with flag: ", shaderCreationInfos.at(index).type, "!");
	}
}

void VulkanPipeline::create_descriptor_stuff(Builder builder) {
	_descriptorSetLayout = new VulkanDescriptorSetLayout;
	_descriptorPool = new VulkanDescriptorPool;

	// configure the builders using the custom pipeline builder
	VulkanDescriptorSetLayout::Builder layoutBuilder; // layout
	layoutBuilder.bindings = builder.bindings;

	VulkanDescriptorPool::Builder poolBuilder; // pool
	poolBuilder.maxSets = builder.maxSets;
	poolBuilder.poolFlags = builder.poolFlags;
	poolBuilder.poolSizes = builder.poolSizes;

	// create the descriptor layout and pool
	_descriptorSetLayout->create(layoutBuilder);
	_descriptorPool->create(poolBuilder);
}

} // namespace lyra
