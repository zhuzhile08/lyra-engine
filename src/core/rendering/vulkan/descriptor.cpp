#include <core/rendering/vulkan/descriptor.h>

namespace lyra {

void VulkanDescriptorSetLayout::create(const Builder builder) {
	Logger::log_info("Creating Vulkan descriptor set layout...");

	// create the descriptor set
	VkDescriptorSetLayoutCreateInfo layoutInfo{
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		nullptr,
		0,
		static_cast<uint32>(builder.bindings.size()),
		builder.bindings.data()
	};

	if (vkCreateDescriptorSetLayout(Application::context()->device()->device(), &layoutInfo, nullptr, &_descriptorSetLayout) != VK_SUCCESS) 
		Logger::log_exception("Failed to create descriptor set layout");

	Logger::log_info("Succesfully created Vulkan descriptor set layout at ", get_address(this), "!", Logger::end_l());
}

// descriptor pool
VulkanDescriptorPool::VulkanDescriptorPool() { }

void VulkanDescriptorPool::create(const Builder builder) {
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

	if (vkCreateDescriptorPool(Application::context()->device()->device(), &poolInfo, nullptr, &_descriptorPool) != VK_SUCCESS) 
		Logger::log_exception("Failed to create descriptor pool");

	Logger::log_info("Succesfully created Vulkan descriptor pool at ", get_address(this), "!", Logger::end_l());
}

// descriptors
VulkanDescriptor::VulkanDescriptor() { }

void VulkanDescriptor::create(const VulkanDescriptorSetLayout* const layout, const VulkanDescriptorPool* const pool, Writer writer) {
	Logger::log_info("Creating Vulkan descriptor sets...");

	// create the descriptor set
	VkDescriptorSetAllocateInfo allocInfo {
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		nullptr,
		pool->get(),
		1,
		layout->get_ptr()
	};

	if (vkAllocateDescriptorSets(Application::context()->device()->device(), &allocInfo, &_descriptorSet) != VK_SUCCESS) Logger::log_exception("Failed to allocate descriptor sets");

	for(auto& write : writer.writes) write.dstSet = _descriptorSet;

	vkUpdateDescriptorSets(Application::context()->device()->device(), static_cast<uint32>(writer.writes.size()), writer.writes.data(), 0, nullptr);

	Logger::log_info("Succesfully created Vulkan descriptor at ", get_address(this), "!", Logger::end_l());
}

} // namespace lyra

