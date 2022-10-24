#include <rendering/vulkan/descriptor.h>




#include <rendering/vulkan/vulkan_shader.h>

#include <core/application.h>

namespace lyra {

namespace vulkan {

DescriptorSetLayout::DescriptorSetLayout(const Builder builder) {
	Logger::log_info("Creating Vulkan descriptor set layout...");

	// create the descriptor set
	VkDescriptorSetLayoutCreateInfo layoutInfo{
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		nullptr,
		0,
		static_cast<uint32>(builder.bindings.size()),
		builder.bindings.data()
	};

	vassert(vkCreateDescriptorSetLayout(Application::renderSystem()->device()->device(), &layoutInfo, nullptr, &m_descriptorSetLayout),
		"create descriptor set layout");

	Logger::log_info("Successfully created Vulkan descriptor set layout at ", get_address(this), "!", Logger::end_l());
}

DescriptorSetLayout::~DescriptorSetLayout() noexcept {
	vkDestroyDescriptorSetLayout(Application::renderSystem()->device()->device(), m_descriptorSetLayout, nullptr);

	Logger::log_info("Successfully destroyed Vulkan descriptor set layout!");
}

// descriptor pool
DescriptorPool::DescriptorPool(const Builder builder) {
	Logger::log_info("Creating Vulkan descriptor pool...");

	// create the descriptor pool
	VkDescriptorPoolCreateInfo 	poolInfo {
		VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		nullptr,
		builder.poolFlags,
		builder.maxSets,
		static_cast<uint32>(builder.poolSizes.size()),
		builder.poolSizes.data()
	};

	vassert(vkCreateDescriptorPool(Application::renderSystem()->device()->device(), &poolInfo, nullptr, &m_descriptorPool),
		"create descriptor pool");

	Logger::log_info("Successfully created Vulkan descriptor pool at ", get_address(this), "!", Logger::end_l());
}

DescriptorPool::~DescriptorPool() noexcept {
	vkDestroyDescriptorPool(Application::renderSystem()->device()->device(), m_descriptorPool, nullptr);

	Logger::log_info("Successfully destroyed Vulkan descriptor pool!");
}

// descriptors
Descriptor::Descriptor(const DescriptorSetLayout* const layout, const DescriptorPool* const pool, Writer writer) {
	Logger::log_info("Creating Vulkan descriptor sets...");

	// create the descriptor set
	VkDescriptorSetAllocateInfo allocInfo {
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		nullptr,
		pool->get(),
		1,
		layout->get_ptr()
	};

	vassert(vkAllocateDescriptorSets(Application::renderSystem()->device()->device(), &allocInfo, &m_descriptorSet), "allocate descriptor sets");

	for(uint32 i = 0; i < writer.writes.size(); i++) writer.writes.at(i).dstSet = m_descriptorSet;

	vkUpdateDescriptorSets(Application::renderSystem()->device()->device(), static_cast<uint32>(writer.writes.size()), writer.writes.data(), 0, nullptr);

	Logger::log_info("Successfully created Vulkan descriptor at ", get_address(this), "!", Logger::end_l());
}

} // namespace vulkan

} // namespace lyra
