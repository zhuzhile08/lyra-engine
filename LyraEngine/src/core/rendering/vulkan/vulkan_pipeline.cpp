#include <core/rendering/vulkan/vulkan_pipeline.h>

#include <core/logger.h>

#include <core/rendering/vulkan/devices.h>
#include <core/rendering/vulkan/vulkan_shader.h>
#include <core/rendering/vulkan/command_buffer.h>
#include <core/rendering/vulkan/descriptor.h>

namespace lyra {

namespace vulkan {

Pipeline::~Pipeline() noexcept {
	// destroy pipeline and layout
	vkDestroyPipeline(Application::renderSystem()->device()->device(), m_pipeline, nullptr);
	vkDestroyPipelineLayout(Application::renderSystem()->device()->device(), m_layout, nullptr);

	Logger::log_info("Successfully destroyed Vulkan pipeline!");
}

void Pipeline::create_layout() {
	// create the pipeline layout
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		nullptr,
		0,
		1,
		m_descriptorSetLayout->get_ptr(),
		0,	/// @todo push constants
		nullptr
	};

	lassert(vkCreatePipelineLayout(Application::renderSystem()->device()->device(), &pipelineLayoutInfo, nullptr, &m_layout) == VK_SUCCESS, "Failed to create Vulkan graphics pipeline layout!");
}

void Pipeline::create_shaders(std::vector<ShaderInfo> shaders) {
	m_shaders.reserve(shaders.size());

#ifdef _DEBUG
	// check if there are the same amount of shaders as shader informations
	if (shaders.size() != m_shaders.size()) {
		Logger::log_warning("Number of shader creation infos doesn't match up with the numbers of shaders in the pipeline at: ", get_address(this), "!");
	}
#endif

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
