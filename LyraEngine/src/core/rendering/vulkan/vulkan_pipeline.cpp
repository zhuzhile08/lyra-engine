#include <core/rendering/vulkan/vulkan_pipeline.h>

#include <core/logger.h>

#include <core/rendering/vulkan/devices.h>
#include <core/rendering/vulkan/vulkan_shader.h>
#include <core/rendering/vulkan/command_buffer.h>
#include <core/rendering/vulkan/descriptor.h>

namespace lyra {

VulkanPipeline::~VulkanPipeline() noexcept {
	vkDestroyPipeline(Context::get()->renderSystem()->device()->device(), _pipeline, nullptr);
	vkDestroyPipelineLayout(Context::get()->renderSystem()->device()->device(), _layout, nullptr);
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

	lassert(vkCreatePipelineLayout(Context::get()->renderSystem()->device()->device(), &pipelineLayoutInfo, nullptr, &_layout) == VK_SUCCESS, "Failed to create Vulkan graphics pipeline layout!");
}

void VulkanPipeline::create_shaders(std::vector<Binding> bindings) {
	_shaders.reserve(bindings.size());

	if (bindings.size() != _shaders.size()) {
		Logger::log_warning("Number of shader creation infos doesn't match up with the numbers of shaders in the pipeline at: ", get_address(this), "!");
	}

	for (int index = 0; index < bindings.size(); index++) {
		_shaders.emplace_back(Context::get()->renderSystem()->device(), bindings.at(index).path, bindings.at(index).entry, 
			static_cast<VulkanShader::Type>(bindings.at(index).shaderType));
		Logger::log_info("Successfully created Vulkan shader at: ", get_address(&_shaders.at(index)), " with flag: ", bindings.at(index).shaderType, "!");
	}
}

void VulkanPipeline::create_descriptor_stuff(std::vector<Binding> bindings, VkDescriptorPoolCreateFlags poolFlags) {
	// create the shaders first
	create_shaders(bindings);

	// configure the builders using the custom pipeline builder
	VulkanDescriptorSetLayout::Builder layoutBuilder; // layout
	VulkanDescriptorPool::Builder poolBuilder; // pool

	uint32 setCount = 0;

	for (uint32 i = 0; i < bindings.size(); i++) {
		// add the information to the layout builder first
		layoutBuilder.add_bindings({ std::make_tuple(
			i,
			bindings.at(i).descriptorType,
			bindings.at(i).shaderType,
			bindings.at(i).descriptorCount
		)});
		// then add the information to the pool builder
		poolBuilder.add_pool_sizes( {{
			bindings.at(i).descriptorType,
			bindings.at(i).descriptorAllocCount
		}});
		// update the count of descriptor sets
		setCount += bindings.at(i).descriptorAllocCount;
	}

	// set some remaining information about the pool
	poolBuilder.maxSets = setCount;
	poolBuilder.poolFlags = poolFlags;

	// create the descriptor layout and pool
	_descriptorSetLayout = std::make_shared<VulkanDescriptorSetLayout>(layoutBuilder);
	_descriptorPool = std::make_shared<VulkanDescriptorPool>(poolBuilder);
}

} // namespace lyra
