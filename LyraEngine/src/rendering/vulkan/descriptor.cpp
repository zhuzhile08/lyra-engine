#include <rendering/vulkan/descriptor.h>

#include <rendering/vulkan/vulkan_shader.h>

#include <core/application.h>

namespace lyra {

namespace vulkan {

DescriptorSystem::DescriptorSetLayouts::DescriptorSetLayouts(const Dynarray<VkDescriptorSetLayoutCreateInfo, 6>& createInfo) {
	// resize the number of descriptor set layouts
	m_descriptorSetLayouts.resize(createInfo.size());

	// create the descriptor set
	for (uint32 i = 0; i < createInfo.size(); i++) {
		vassert(vkCreateDescriptorSetLayout(Application::renderSystem.device.device(), &createInfo[i], nullptr, &m_descriptorSetLayouts[i]),
			"create descriptor set layout");
	}
}

DescriptorSystem::DescriptorSetLayouts::~DescriptorSetLayouts() noexcept {
	for (uint32 i = 0; i < m_descriptorSetLayouts.size(); i++)
		vkDestroyDescriptorSetLayout(Application::renderSystem.device.device(), m_descriptorSetLayouts[i], nullptr);
}

// descriptor pool
DescriptorSystem::DescriptorPool::DescriptorPool(const VkDescriptorPoolCreateInfo& createInfo) {
	// create the descriptor pool
	vassert(vkCreateDescriptorPool(Application::renderSystem.device.device(), &createInfo, nullptr, &m_descriptorPool),
		"create descriptor pool");
}

DescriptorSystem::DescriptorPool::~DescriptorPool() noexcept {
	vkDestroyDescriptorPool(Application::renderSystem.device.device(), m_descriptorPool, nullptr);
}

// descriptors
DescriptorSystem::DescriptorSet::DescriptorSet(const VkDescriptorSetAllocateInfo& allocInfo, BasicPool<DescriptorSet>& pool)
	: ResourceBase(pool) {
	// create the descriptor set
	vassert(vkAllocateDescriptorSets(Application::renderSystem.device.device(), &allocInfo, &m_descriptorSet), "allocate descriptor sets");
}

void DescriptorSystem::DescriptorSet::update() const noexcept {
	Application::renderSystem.device.updateDescriptorSets(static_cast<uint32>(m_writes.size()), m_writes.data());
}

// layout builder
void DescriptorSystem::LayoutBuilder::add_binding(const DescriptorSystem::DescriptorSetLayouts::Data& newBinding) {
	// first, check if the number of existing binding sets is smaller than the index of the new binding to be added
	// if so, then add one set of bindings
	if(m_bindings.size() < newBinding.layoutIndex) {
		m_bindings.push_back(std::vector<VkDescriptorSetLayoutBinding> { });
	}

	// finally add the new binding
	m_bindings[newBinding.layoutIndex].push_back({
		newBinding.binding,
		static_cast<VkDescriptorType>(newBinding.type),
		newBinding.arraySize,
		static_cast<VkShaderStageFlags>(newBinding.shaderType),
		nullptr
	});
}

const Dynarray<VkDescriptorSetLayoutCreateInfo, 6> DescriptorSystem::LayoutBuilder::build_create_info() const {
	// create a dynarray of results
	Dynarray<VkDescriptorSetLayoutCreateInfo, 6> createInfos;
	
	// make the create infos out of the bindings and push them into the dynarray
	for (uint32 i = 0; i <= m_bindings.size(); i++) {
		createInfos.push_back(VkDescriptorSetLayoutCreateInfo {
			VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			nullptr,
			0,
			static_cast<uint32>(m_bindings[i].size()),
			m_bindings[i].data()
		});
	}

	// return the creation information
	return createInfos;
}

DescriptorSystem::DescriptorSet& DescriptorSystem::get_unused_set(const size_t& layoutIndex) {
#ifndef NDEBUG
	lyra::lassert(layoutIndex >= m_layouts->size(), "Requested descriptor using a non-existing layout (index) !");
#endif
	if (m_sets[layoutIndex].unused_size() == 0) {
		create_descriptor_pool(layoutIndex);
	}

	return m_sets[layoutIndex].get_unused();
}

void DescriptorSystem::create_descriptor_pool(const uint32& layoutIndex) {
	// first create the pool
	m_pools.push_back(m_poolBuilder->build_create_info());

	VkDescriptorSetAllocateInfo allocInfo {
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		nullptr,
		m_pools.back(),
		1,
		&m_layouts->get()[layoutIndex]
	};

	for (uint32 i = 0; i <= Settings::MemConfig::maxDescriptorTypePerPool; i++) {
		m_sets[layoutIndex].emplace_back(allocInfo, m_sets[layoutIndex]);
	}
}

} // namespace vulkan

} // namespace lyra

