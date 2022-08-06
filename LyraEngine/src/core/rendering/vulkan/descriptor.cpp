#include <core/rendering/vulkan/descriptor.h>

#include <core/logger.h>

#include <core/rendering/vulkan/vulkan_shader.h>

namespace lyra {

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(const Builder builder) {
	Logger::log_info("Creating Vulkan descriptor set layout...");

	// create the descriptor set
	VkDescriptorSetLayoutCreateInfo layoutInfo{
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		nullptr,
		0,
		static_cast<uint32>(builder.bindings.size()),
		builder.bindings.data()
	};

	lassert(vkCreateDescriptorSetLayout(Context::get()->renderSystem()->device()->device(), &layoutInfo, nullptr, &_descriptorSetLayout) == VK_SUCCESS,
		"Failed to create descriptor set layout");

	Logger::log_info("Successfully created Vulkan descriptor set layout at ", get_address(this), "!", Logger::end_l());
}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout() noexcept {
	vkDestroyDescriptorSetLayout(Context::get()->renderSystem()->device()->device(), _descriptorSetLayout, nullptr);

	Logger::log_info("Successfully destroyed Vulkan descriptor set layout!");
}

// descriptor pool
VulkanDescriptorPool::VulkanDescriptorPool(const Builder builder) {
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

	lassert(vkCreateDescriptorPool(Context::get()->renderSystem()->device()->device(), &poolInfo, nullptr, &_descriptorPool) == VK_SUCCESS,
		"Failed to create descriptor pool");

	Logger::log_info("Successfully created Vulkan descriptor pool at ", get_address(this), "!", Logger::end_l());
}

VulkanDescriptorPool::~VulkanDescriptorPool() noexcept {
	vkDestroyDescriptorPool(Context::get()->renderSystem()->device()->device(), _descriptorPool, nullptr);

	Logger::log_info("Successfully destroyed Vulkan descriptor pool!");
}

// descriptors
VulkanDescriptor::VulkanDescriptor(const VulkanDescriptorSetLayout* const layout, const VulkanDescriptorPool* const pool, Writer writer) {
	Logger::log_info("Creating Vulkan descriptor sets...");

	// create the descriptor set
	VkDescriptorSetAllocateInfo allocInfo {
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		nullptr,
		pool->get(),
		1,
		layout->get_ptr()
	};

	lassert(vkAllocateDescriptorSets(Context::get()->renderSystem()->device()->device(), &allocInfo, &_descriptorSet) == VK_SUCCESS, "Failed to allocate descriptor sets");

	for(uint32 i = 0; i < writer.writes.size(); i++) writer.writes.at(i).dstSet = _descriptorSet;

	vkUpdateDescriptorSets(Context::get()->renderSystem()->device()->device(), static_cast<uint32>(writer.writes.size()), writer.writes.data(), 0, nullptr);

	Logger::log_info("Successfully created Vulkan descriptor at ", get_address(this), "!", Logger::end_l());
}

} // namespace lyra

