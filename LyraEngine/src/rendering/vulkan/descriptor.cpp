#include <rendering/vulkan/descriptor.h>

#include <rendering/vulkan/vulkan_shader.h>

#include <core/application.h>

namespace lyra {

namespace vulkan {

DescriptorSetLayout::DescriptorSetLayout(const std::vector<Builder>& builders) {
	m_descriptorSetLayouts.resize(builders.size());

	// create the descriptor set
	for (uint32 i = 0; i < builders.size(); i++) {
		VkDescriptorSetLayoutCreateInfo layoutInfo{
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			nullptr,
			0,
			static_cast<uint32>(builders.at(i).bindings.size()),
			builders.at(i).bindings.data()
		};

		vassert(vkCreateDescriptorSetLayout(Application::renderSystem.device.device(), &layoutInfo, nullptr, &m_descriptorSetLayouts[i]),
			"create descriptor set layout");
	}
}

DescriptorSetLayout::~DescriptorSetLayout() noexcept {
	for (uint32 i = 0; i < m_descriptorSetLayouts.size(); i++)
		vkDestroyDescriptorSetLayout(Application::renderSystem.device.device(), m_descriptorSetLayouts[i], nullptr);
}

// descriptor pool
DescriptorPool::DescriptorPool(const Builder& builder) {
	// create the descriptor pool
	VkDescriptorPoolCreateInfo 	poolInfo {
		VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		nullptr,
		builder.poolFlags,
		builder.maxSets,
		static_cast<uint32>(builder.poolSizes.size()),
		builder.poolSizes.data()
	};

	vassert(vkCreateDescriptorPool(Application::renderSystem.device.device(), &poolInfo, nullptr, &m_descriptorPool),
		"create descriptor pool");
}

DescriptorPool::~DescriptorPool() noexcept {
	vkDestroyDescriptorPool(Application::renderSystem.device.device(), m_descriptorPool, nullptr);
}

// descriptors
Descriptor::Descriptor(const DescriptorSetLayout* const layout, const uint32& layoutIndex, const DescriptorPool* const pool, Writer& writer) {
	// create the descriptor set
	VkDescriptorSetAllocateInfo allocInfo {
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		nullptr,
		pool->get(),
		1,
		&layout->m_descriptorSetLayouts.at(layoutIndex)
	};

	vassert(vkAllocateDescriptorSets(Application::renderSystem.device.device(), &allocInfo, &m_descriptorSet), "allocate descriptor sets");

	for (uint32 i = 0; i < writer.writes.size(); i++) writer.writes.at(i).dstSet = m_descriptorSet;

	Application::renderSystem.device.updateDescriptorSets(static_cast<uint32>(writer.writes.size()), writer.writes.data());
}

} // namespace vulkan

} // namespace lyra

