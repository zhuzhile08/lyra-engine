#include <core/rendering/vulkan/descriptor.h>

namespace lyra {

// descriptor set layout builder
VulkanDescriptorSetLayout::Builder::Builder() { }

VulkanDescriptorSetLayout::Builder::~Builder() {
	bindings.clear();
}

void VulkanDescriptorSetLayout::Builder::add_binding(const uint32 binding, const VkDescriptorType type, const VkShaderStageFlags stage, const uint32 count) noexcept {
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

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout() {
	vkDestroyDescriptorSetLayout(device->device(), _descriptorSetLayout, nullptr);

	LOG_INFO("Succesfully destroyed Vulkan descriptor set layout!");
}

void VulkanDescriptorSetLayout::destroy() noexcept {
	this->~VulkanDescriptorSetLayout();
}

void VulkanDescriptorSetLayout::create(const VulkanDevice* device, const Builder builder) {
	LOG_INFO("Creating Vulkan descriptor set layout...");

	this->device = device;

	// create the descriptor set
	VkDescriptorSetLayoutCreateInfo layoutInfo{
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		nullptr,
		0,
		static_cast<uint32>(builder.bindings.size()),
		builder.bindings.data()
	};

	if (vkCreateDescriptorSetLayout(device->device(), &layoutInfo, nullptr, &_descriptorSetLayout) != VK_SUCCESS) LOG_EXEPTION("Failed to create descriptor set layout");

	LOG_INFO("Succesfully created Vulkan descriptor set layout at ", GET_ADDRESS(this), "!", END_L);
}

// descriptor pool builder
VulkanDescriptorPool::Builder::Builder() { }

VulkanDescriptorPool::Builder::~Builder() {
	poolSizes.clear();
}

void VulkanDescriptorPool::Builder::add_pool_sizes(const VkDescriptorType type, const uint32 count) noexcept {
	poolSizes.push_back({
		type,
		count
	});
}

void VulkanDescriptorPool::Builder::set_max_sets(uint32 _maxSets) noexcept {
	maxSets = _maxSets;
}

void VulkanDescriptorPool::Builder::set_pool_flags(VkDescriptorPoolCreateFlags _poolFlags) noexcept {
	poolFlags = _poolFlags;
}

// descriptor pool
VulkanDescriptorPool::VulkanDescriptorPool() { }

VulkanDescriptorPool::~VulkanDescriptorPool() {
	vkDestroyDescriptorPool(device->device(), _descriptorPool, nullptr);

	LOG_INFO("Succesfully destroyed Vulkan descriptor pool!");
}

void VulkanDescriptorPool::destroy() noexcept {
	this->~VulkanDescriptorPool();
}

void VulkanDescriptorPool::create(const VulkanDevice* device, const Builder builder) {
	LOG_INFO("Creating Vulkan descriptor pool...");

		this->device = device;

	// create the descriptor pool
	VkDescriptorPoolCreateInfo 	poolInfo {
		VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		nullptr,
		builder.poolFlags,
		builder.maxSets,
		static_cast<uint32>(builder.poolSizes.size()),
		builder.poolSizes.data()
	};

	if (vkCreateDescriptorPool(device->device(), &poolInfo, nullptr, &_descriptorPool) != VK_SUCCESS) LOG_EXEPTION("Failed to create descriptor pool");

	LOG_INFO("Succesfully created Vulkan descriptor pool at ", GET_ADDRESS(this), "!", END_L);
}

// descriptor writer
VulkanDescriptor::Writer::Writer() { }

VulkanDescriptor::Writer::~Writer() {
	writes.clear();
}

void VulkanDescriptor::Writer::add_buffer_write(const VkDescriptorBufferInfo *bufferInfo, const uint16 binding, const VkDescriptorType type) noexcept {
	writes.push_back({
		VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		nullptr,
		VK_NULL_HANDLE,
		binding,
		0,
		1,
		type,
		nullptr,
		bufferInfo,
		nullptr
	});
}

void VulkanDescriptor::Writer::add_image_write(const VkDescriptorImageInfo *imageInfo, const uint16 binding, const VkDescriptorType type) noexcept {
	writes.push_back({
		VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		nullptr,
		VK_NULL_HANDLE,
		binding,
		0,
		1,
		type,
		imageInfo,
		nullptr,
		nullptr
	});
}

// descriptors
VulkanDescriptor::VulkanDescriptor() { }

void VulkanDescriptor::create(const VulkanDevice* device, const VulkanDescriptorSetLayout layout, const VulkanDescriptorPool pool, Writer writer) {
	LOG_INFO("Creating Vulkan descriptor sets...");

	// create the descriptor set
	VkDescriptorSetAllocateInfo allocInfo {
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		nullptr,
		pool.get(),
		1,
		layout.get_ptr()
	};

	if (vkAllocateDescriptorSets(device->device(), &allocInfo, &_descriptorSet) != VK_SUCCESS) LOG_EXEPTION("Failed to allocate descriptor sets");

	for(auto& write : writer.writes) write.dstSet = _descriptorSet;

	vkUpdateDescriptorSets(device->device(), 1, writer.writes.data(), 0, nullptr);

	LOG_INFO("Succesfully created Vulkan descriptor at ", GET_ADDRESS(this), "!", END_L);
}

} // namespace lyra

