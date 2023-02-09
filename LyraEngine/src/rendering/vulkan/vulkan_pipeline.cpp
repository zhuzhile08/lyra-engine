#include <rendering/vulkan/vulkan_pipeline.h>

#include <rendering/vulkan/devices.h>
#include <rendering/vulkan/descriptor.h>
#include <rendering/vulkan/command_buffer.h>
#include <rendering/vulkan/vulkan_shader.h>

#include <core/application.h>

namespace lyra {

namespace vulkan {

const VkPipelineLayoutCreateInfo Pipeline::Builder::build_layout_create_info(const DescriptorSystem& descriptorSystem) const noexcept {
	// vector of push constant data(s)
	std::vector<VkPushConstantRange> pushConstantData(m_pushConstantInfos.size());
	// convert my own internal push constant data structure to the vulkan version of it
	for (const auto& pushConstantInfo : m_pushConstantInfos) {
		pushConstantData.push_back(VkPushConstantRange {
			static_cast<uint32>(pushConstantInfo.shaderType),
			pushConstantInfo.offset,
			pushConstantInfo.size
		});
	}
	
	// return the creation information
	return VkPipelineLayoutCreateInfo {
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		nullptr,
		0,
		static_cast<uint32>(descriptorSystem.layouts().size()),
		descriptorSystem.layouts(),
		static_cast<uint32>(pushConstantData.size()),
		pushConstantData.data()
	};
}

const DescriptorSystem::LayoutBuilder Pipeline::Builder::build_set_layout_builder() const {
	// descriptor set layout builder
	DescriptorSystem::LayoutBuilder layoutBuilder; 

	// add the information to the layout builder first
	for (uint32 i = 0; i < m_bindingInfos.size(); i++) {
		layoutBuilder.add_binding({
			m_bindingInfos[i].descriptorSetLayoutIndex,
			m_bindingInfos[i].shaderType,
			i,
			m_bindingInfos[i].arraySize,
			m_bindingInfos[i].descriptorType
		});
	}

	// return the builder
	return layoutBuilder;
}

const DescriptorSystem::PoolBuilder Pipeline::Builder::build_pool_builder() const {
	// descriptor pool builder
	DescriptorSystem::PoolBuilder poolBuilder;
	
	// add the information to the pool builder
	for (const auto& bindingInfo : m_bindingInfos) {
		poolBuilder.add_pool_size({
			bindingInfo.descriptorType,
			bindingInfo.descriptorAllocCountMultiplier
		});
	}
	// set some remaining information about the pool
	poolBuilder.set_pool_flags(m_poolFlags);

	// return the builder
	return poolBuilder;
}

Pipeline::~Pipeline() noexcept {
	// destroy pipeline and layout
	vkDestroyPipeline(Application::renderSystem.device.device(), m_pipeline, nullptr);
	vkDestroyPipelineLayout(Application::renderSystem.device.device(), m_layout, nullptr);
}

void Pipeline::create_layout(const Builder* const builder) {
	// create the pipeline layout
	const auto& createInfo = builder->build_layout_create_info(m_descriptorSystem);
	vassert(vkCreatePipelineLayout(Application::renderSystem.device.device(), &createInfo, nullptr, &m_layout), "create Vulkan graphics pipeline layout");
}

void Pipeline::create_shaders(const Builder* const builder) {
	m_shaders.reserve(builder->m_shaderInfos.size());

	// create the shaders in the vector
	for (const auto& shaderInfo : builder->m_shaderInfos) {
		m_shaders.emplace_back(shaderInfo.path, shaderInfo.entry,
			static_cast<Shader::Type>(shaderInfo.type));
	}
}

void Pipeline::create_descriptor_stuff(const Builder* const builder) {
	// create the descriptor system's layout and the pool builder template
	m_descriptorSystem.create_descriptor_set_layout(builder->build_set_layout_builder(), builder->build_pool_builder());
}

} // namespace vulkan

} // namespace lyra
