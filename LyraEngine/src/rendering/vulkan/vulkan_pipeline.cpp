#include <rendering/vulkan/vulkan_pipeline.h>

#include <core/logger.h>

#include <rendering/vulkan/devices.h>
#include <rendering/vulkan/command_buffer.h>
#include <rendering/vulkan/descriptor.h>
#include <rendering/vulkan/vulkan_shader.h>

#include <core/application.h>

namespace lyra {

namespace vulkan {

Pipeline::~Pipeline() noexcept {
	// destroy pipeline and layout
	vkDestroyPipeline(Application::renderSystem()->device()->device(), m_pipeline, nullptr);
	vkDestroyPipelineLayout(Application::renderSystem()->device()->device(), m_layout, nullptr);

	Logger::log_info("Successfully destroyed Vulkan pipeline!");
}

void Pipeline::create_layout(std::vector<VkPushConstantRange> pushConstants) {
	// create the pipeline layout
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		nullptr,
		0,
		1,
		m_descriptorSetLayout->get_ptr(),
		(uint32) pushConstants.size(),	/// @todo push constants
		pushConstants.data()
	};

	vassert(vkCreatePipelineLayout(Application::renderSystem()->device()->device(), &pipelineLayoutInfo, nullptr, &m_layout), "create Vulkan graphics pipeline layout");
}

void Pipeline::create_shaders(std::vector<ShaderInfo> shaders) {
	m_shaders.reserve(shaders.size());

	// create the shaders in the vector
	for (int index = 0; index < shaders.size(); index++) {
		m_shaders.emplace_back(shaders.at(index).path, shaders.at(index).entry,
			static_cast<Shader::Type>(shaders.at(index).type));
		Logger::log_info("Successfully created Vulkan shader at: ", get_address(&m_shaders.at(index)), " with flag: ", shaders.at(index).type, "!");
	}
}

void Pipeline::create_descriptor_stuff(std::vector<Binding> bindings, VkDescriptorPoolCreateFlags poolFlags) {
	// configure the builders using the custom pipeline builder
	DescriptorSetLayout::Builder layoutBuilder; // layout
	DescriptorPool::Builder poolBuilder; // pool

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
		poolBuilder.maxSets += bindings.at(i).descriptorAllocCount;
	}

	// set some remaining information about the pool
	poolBuilder.poolFlags = poolFlags;

	// create the descriptor layout and pool
	m_descriptorSetLayout = SmartPointer<DescriptorSetLayout>::create(layoutBuilder);
	m_descriptorPool = SmartPointer<DescriptorPool>::create(poolBuilder);
}

} // namespace vulkan

} // namespace lyra
