#include <rendering/vulkan/descriptor.h>

namespace lyra {

// descriptor set layout builder
VulkanDescriptorSetLayout::Builder::Builder() { }

void VulkanDescriptorSetLayout::Builder::add_binding(const uint32 binding, const VkDescriptorType type, const VkShaderStageFlags stage, const uint32 count) {
    bindings.push_back({
        binding,
        type,
        count,
        stage,
        nullptr
    });
}

// descriptor set layout
VulkanDescriptorSetLayout::VulkanDescriptorSetLayout() { }

void VulkanDescriptorSetLayout::destroy() {
    vkDestroyDescriptorSetLayout(device->get().device, descriptorSetLayout, nullptr);

    LOG_INFO("Succesfully destroyed Vulkan descriptor set layout!")
}

void VulkanDescriptorSetLayout::create(VulkanDevice device, const Builder builder) {
    LOG_INFO("Creating Vulkan descriptor set layout...")

    this->device = new VulkanDevice(device);

	// create the descriptor set
	VkDescriptorSetLayoutCreateInfo layoutInfo {
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		nullptr,
		0,
		1,
		builder.bindings.data()
	};

	if(vkCreateDescriptorSetLayout(device.get().device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) LOG_EXEPTION("Failed to create descriptor set layout")

    LOG_INFO("Succesfully created Vulkan descriptor set layout at ", GET_ADDRESS(this), "!", END_L)
}

VkDescriptorSetLayout VulkanDescriptorSetLayout::get() const {
	return descriptorSetLayout;
}

const VkDescriptorSetLayout* VulkanDescriptorSetLayout::get_ptr() const {
	return &descriptorSetLayout;
}

// descriptor pool builder
VulkanDescriptorPool::Builder::Builder() { }

void VulkanDescriptorPool::Builder::add_pool_sizes(const VkDescriptorType type, const uint32_t count) {
    poolSizes.push_back({
        type,
        count
    });
}

void VulkanDescriptorPool::Builder::set_max_sets(uint32 _maxSets) {
    maxSets = _maxSets;
}

void VulkanDescriptorPool::Builder::set_pool_flags(VkDescriptorPoolCreateFlags _poolFlags) {
    poolFlags = _poolFlags;
}

// descriptor pool
VulkanDescriptorPool::VulkanDescriptorPool() { }

void VulkanDescriptorPool::destroy() {
	vkDestroyDescriptorPool(device->get().device, descriptorPool, nullptr);

	LOG_INFO("Succesfully destroyed Vulkan descriptor pool!")
}

void VulkanDescriptorPool::create(VulkanDevice device, const Builder builder) {
    LOG_INFO("Creating Vulkan descriptor pool...")

    // create the descriptor pool
    VkDescriptorPoolCreateInfo 	poolInfo {
        VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        nullptr,
        builder.poolFlags,
        builder.maxSets,
        static_cast<uint32>(builder.poolSizes.size()),
        builder.poolSizes.data()
    };

    if(vkCreateDescriptorPool(device.get().device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) LOG_EXEPTION("Failed to create descriptor pool");

	LOG_INFO("Succesfully created Vulkan descriptor pool at ", GET_ADDRESS(this), "!", END_L)
}

VkDescriptorPool VulkanDescriptorPool::get() const {
	return descriptorPool;
}

// descriptor writer
VulkanDescriptor::Writer::Writer() { }

void VulkanDescriptor::Writer::add_buffer_write(const VkDescriptorBufferInfo bufferInfo, const uint16 binding, const VkDescriptorType type) {
    write = {
        VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        nullptr,
        VK_NULL_HANDLE,
        binding,
        0,
        1,
        type,
        nullptr,
        &bufferInfo,
        nullptr
    };
}

void VulkanDescriptor::Writer::add_image_write(const VkDescriptorImageInfo imageInfo, const uint16 binding, const VkDescriptorType type) {
    write = {
        VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        nullptr,
        VK_NULL_HANDLE,
        binding,
        0,
        1,
        type,
        &imageInfo,
        nullptr,
        nullptr
    };
}

// descriptors
VulkanDescriptor::VulkanDescriptor() { }

void VulkanDescriptor::create(VulkanDevice device, const VulkanDescriptorSetLayout layout, const VulkanDescriptorPool pool, Writer writer) {
    LOG_INFO("Creating Vulkan descriptor sets...")

    // create the descriptor set
    VkDescriptorSetAllocateInfo allocInfo {
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        nullptr,
        pool.get(),
        1,
        layout.get_ptr()
    };

    if (vkAllocateDescriptorSets(device.get().device, &allocInfo, &descriptorSet) != VK_SUCCESS) LOG_EXEPTION("Failed to allocate descriptor sets")

    writer.write.dstSet = descriptorSet;

    vkUpdateDescriptorSets(device.get().device, 1, &writer.write, 0, nullptr);

    LOG_INFO("Succesfully created Vulkan descriptor at ", GET_ADDRESS(this), "!", END_L)
}

VkDescriptorSet VulkanDescriptor::get() const {
    return descriptorSet;
}

const VkDescriptorSet* VulkanDescriptor::get_ptr() const {
    return &descriptorSet;
}

} // namespace lyra

