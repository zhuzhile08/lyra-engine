#include <Graphics/VulkanImpl/DescriptorSystem.h>

#include <Graphics/VulkanImpl/Shader.h>

#include <Application/Application.h>

namespace lyra {

namespace vulkan {

// layout builder
void DescriptorSystem::LayoutBuilder::add_binding(const DescriptorSystem::DescriptorSetLayout::Data& newBinding) {
	// add the new binding
	m_bindings.push_back({
		newBinding.binding,
		static_cast<VkDescriptorType>(newBinding.type),
		newBinding.arraySize,
		static_cast<VkShaderStageFlags>(newBinding.shaderType),
		nullptr
	});
}

// descriptor layout(s)
DescriptorSystem::DescriptorSetLayout::DescriptorSetLayout(const VkDescriptorSetLayoutCreateInfo& createInfo) {
	// create the descriptor set layout
	m_descriptorSetLayout = vk::DescriptorSetLayout(Application::renderSystem.device.device(), createInfo);
}


// descriptor pool
DescriptorSystem::DescriptorPool::DescriptorPool(const VkDescriptorPoolCreateInfo& createInfo) : m_capacity(createInfo.maxSets) {
	// create the descriptor pool
	m_descriptorPool = vk::DescriptorPool(Application::renderSystem.device.device(), createInfo);
}

void DescriptorSystem::create_descriptor_pool() {
	// set allocation information, creates the pool too
	VkDescriptorSetAllocateInfo allocInfo {
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		nullptr,
		m_pools.emplace_back(m_poolBuilder.build_create_info()).get(),
		1,
		&m_layout.get().get()
	};

	// create the all descriptor sets belonging to the pool
	for (uint32 i = 0; i < m_pools.back().capacity(); i++) {
		m_sets.emplace_back(allocInfo);
	}
}


// descriptor sets
DescriptorSystem::DescriptorSet::DescriptorSet(const VkDescriptorSetAllocateInfo& allocInfo) {
	// create the descriptor set
	vassert(vkAllocateDescriptorSets(Application::renderSystem.device.device(), &allocInfo, &m_descriptorSet), "allocate descriptor sets");
}

void DescriptorSystem::DescriptorSet::update() const noexcept {
	Application::renderSystem.device.updateDescriptorSets(static_cast<uint32>(m_writes.size()), m_writes.data());
}

DescriptorSystem::DescriptorSetResource DescriptorSystem::get_unused_set() {
	if (m_sets.size() <= 0) {
		create_descriptor_pool();
	}
	return m_sets.get_resource();
}

} // namespace vulkan

} // namespace lyra

