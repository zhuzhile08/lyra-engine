#include <rendering/vulkan/vulkan_pipeline.h>

#include <rendering/vulkan/devices.h>
#include <rendering/vulkan/descriptor.h>
#include <rendering/vulkan/command_buffer.h>
#include <rendering/vulkan/vulkan_shader.h>

#include <core/application.h>

namespace lyra {

namespace vulkan {

void Pipeline::Builder::build_pipeline_base(Pipeline* pipeline) const noexcept {
	// create the shaders
	pipeline->m_shaders.reserve(m_shaderInfos.size());
	for (const auto& shaderInfo : m_shaderInfos) {
		pipeline->m_shaders.emplace_back(
			shaderInfo.path, 
			shaderInfo.entry,
			static_cast<Shader::Type>(shaderInfo.type)
		);
	}

	// descriptor set layout buildera
	std::vector<DescriptorSystem::LayoutBuilder> layoutBuilders; 
	// descriptor pool builders
	std::vector<DescriptorSystem::PoolBuilder> poolBuilders;

	// add the information to the layout builder first
	for (const auto& bindingInfo : m_bindingInfos) {
		// get the current index
		size_t i = &bindingInfo - &m_bindingInfos[0];

		// first, check if the current layout index is larger than the size of the layout or pool builder
		// if so, just create a new one and push it to the back of the corresponding vector
		if (m_bindingInfos[i].descriptorSetLayoutIndex >= layoutBuilders.size()) {
			layoutBuilders.push_back(DescriptorSystem::LayoutBuilder { } );
		}
		if (m_bindingInfos[i].descriptorSetLayoutIndex >= poolBuilders.size()) {
			poolBuilders.push_back(DescriptorSystem::PoolBuilder { } );
		}

		// at last, add the bindings and pool sizes
		layoutBuilders[bindingInfo.descriptorSetLayoutIndex].add_binding({
			bindingInfo.shaderType,
			i,
			bindingInfo.arraySize,
			bindingInfo.descriptorType
		});
		poolBuilders[bindingInfo.descriptorSetLayoutIndex].add_pool_size({
			bindingInfo.descriptorType,
			bindingInfo.descriptorAllocCountMultiplier
		});

		// set some remaining information about the pool
		poolBuilders[bindingInfo.descriptorSetLayoutIndex].set_pool_flags((m_poolFlags.size() > bindingInfo.descriptorSetLayoutIndex) ? m_poolFlags[bindingInfo.descriptorSetLayoutIndex] : 0); //@todo this is not very efficient
	}

	// vector of descriptor set layouts
	std::vector<VkDescriptorSetLayout> descriptorSetLayouts;

	// finally, create the descriptor systems
	// first, resize the vector
	pipeline->m_descriptorSystems.resize(layoutBuilders.size());
	// them create them one by one
	for (auto& descriptorSystem : pipeline->m_descriptorSystems) {
		size_t i = &descriptorSystem - &pipeline->m_descriptorSystems[0];
		descriptorSystem = DescriptorSystem(layoutBuilders[i], poolBuilders[i]);
		// also assign the temporary descriptor set layouts for future operations
		descriptorSetLayouts.push_back(descriptorSystem.layout().get());
	}

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
	
	// pipeline layout creation information
	VkPipelineLayoutCreateInfo createInfo {
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		nullptr,
		0,
		static_cast<uint32>(descriptorSetLayouts.size()),
		descriptorSetLayouts.data(),
		static_cast<uint32>(pushConstantData.size()),
		pushConstantData.data()
	};

	// create the pipeline layout
	pipeline->m_layout = vk::PipelineLayout(Application::renderSystem.device.device(), createInfo);
}

} // namespace vulkan

} // namespace lyra
