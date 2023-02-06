#include <rendering/vulkan/vulkan_pipeline.h>

#include <rendering/vulkan/devices.h>
#include <rendering/vulkan/descriptor.h>
#include <rendering/vulkan/command_buffer.h>
#include <rendering/vulkan/vulkan_shader.h>

#include <core/application.h>

namespace lyra {

namespace vulkan {

Pipeline::~Pipeline() noexcept {
	// destroy pipeline and layout
	vkDestroyPipeline(Application::renderSystem.device.device(), m_pipeline, nullptr);
	vkDestroyPipelineLayout(Application::renderSystem.device.device(), m_layout, nullptr);
}

void Pipeline::create_layout(const std::vector<VkPushConstantRange>& pushConstants) {
	// create the pipeline layout
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		nullptr,
		0,
		(uint32) m_descriptorSystem.layouts().size(),
		m_descriptorSystem.layouts(),
		(uint32) pushConstants.size(),
		pushConstants.data()
	};

	vassert(vkCreatePipelineLayout(Application::renderSystem.device.device(), &pipelineLayoutInfo, nullptr, &m_layout), "create Vulkan graphics pipeline layout");
}

void Pipeline::create_shaders(const std::vector<ShaderInfo>& shaderInfo) {
	m_shaders.reserve(shaderInfo.size());

	// create the shaders in the vector
	for (uint32 i = 0; i < shaderInfo.size(); i++) {
		m_shaders.emplace_back(shaderInfo[i].path, shaderInfo[i].entry,
			static_cast<Shader::Type>(shaderInfo[i].type));
	}
}

void Pipeline::create_descriptor_stuff(const std::vector<Binding>& bindings, const VkDescriptorPoolCreateFlags& poolFlags) {
	// configure the builders
	DescriptorSystem::LayoutBuilder layoutBuilder; // layout
	DescriptorSystem::PoolBuilder poolBuilder; // pool

	for (uint32 i = 0; i < bindings.size(); i++) {
		// add the information to the layout builder first
		layoutBuilder.add_binding({
			bindings[i].descriptorSetLayoutIndex,
			bindings[i].shaderType,
			i,
			bindings[i].arraySize,
			bindings[i].descriptorType
		});
		// then add the information to the pool builder
		poolBuilder.add_pool_size({
			bindings[i].descriptorType,
			bindings[i].descriptorAllocCountMultiplier
		});
	}

	// set some remaining information about the pool
	poolBuilder.set_pool_flags(poolFlags);

	// create the descriptor system's layout
	m_descriptorSystem.create_descriptor_set_layout(layoutBuilder, poolBuilder);
}

} // namespace vulkan

} // namespace lyra
