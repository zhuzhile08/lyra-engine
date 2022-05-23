#include <core/rendering/vulkan/descriptor.h>

namespace lyra {

// descriptor set layout builder
VulkanDescriptorSetLayout::Builder::Builder() { }

VulkanDescriptorSetLayout::Builder::~Builder() {
	bindings.clear();
}

void VulkanDescriptorSetLayout::Builder::add_binding(const uint32 binding, const Type type, const VkShaderStageFlags stage, const uint32 count) noexcept {
	bindings.push_back({
		binding,
		static_cast<VkDescriptorType>(type),
		count,
		stage,
		nullptr
	});
}

// descriptor set layout
VulkanDescriptorSetLayout::VulkanDescriptorSetLayout() { }

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout() {
	vkDestroyDescriptorSetLayout(device->device(), _descriptorSetLayout, nullptr);

	Logger::log_info("Succesfully destroyed Vulkan descriptor set layout!");
}

void VulkanDescriptorSetLayout::destroy() noexcept {
	this->~VulkanDescriptorSetLayout();
}

void VulkanDescriptorSetLayout::create(const VulkanDevice* const device, const Builder builder) {
	Logger::log_info("Creating Vulkan descriptor set layout...");

	this->device = device;

	// create the descriptor set
	VkDescriptorSetLayoutCreateInfo layoutInfo{
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		nullptr,
		0,
		static_cast<uint32>(builder.bindings.size()),
		builder.bindings.data()
	};

	if (vkCreateDescriptorSetLayout(device->device(), &layoutInfo, nullptr, &_descriptorSetLayout) != VK_SUCCESS) Logger::log_exception("Failed to create descriptor set layout");

	Logger::log_info("Succesfully created Vulkan descriptor set layout at ", get_address(this), "!", Logger::end_l());
}

// descriptor pool builder
VulkanDescriptorPool::Builder::Builder() { }

VulkanDescriptorPool::Builder::~Builder() {
	poolSizes.clear();
}

void VulkanDescriptorPool::Builder::add_pool_sizes(std::vector<std::pair<const Type, const uint32>> sizes) noexcept {
	for (const auto& [type, size] : sizes) {
		poolSizes.push_back({
			static_cast<VkDescriptorType>(type),
			size
		});
	}
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

	Logger::log_info("Succesfully destroyed Vulkan descriptor pool!");
}

void VulkanDescriptorPool::destroy() noexcept {
	this->~VulkanDescriptorPool();
}

void VulkanDescriptorPool::create(const VulkanDevice* const device, const Builder builder) {
	Logger::log_info("Creating Vulkan descriptor pool...");

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

	if (vkCreateDescriptorPool(device->device(), &poolInfo, nullptr, &_descriptorPool) != VK_SUCCESS) Logger::log_exception("Failed to create descriptor pool");

	Logger::log_info("Succesfully created Vulkan descriptor pool at ", get_address(this), "!", Logger::end_l());
}

// descriptor writer
VulkanDescriptor::Writer::Writer() { }

VulkanDescriptor::Writer::~Writer() {
	writes.clear();
}

void VulkanDescriptor::Writer::add_buffer_write(const VkDescriptorBufferInfo* const bufferInfo, const uint16 binding, const Type type) noexcept {
	writes.push_back({
		VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		nullptr,
		VK_NULL_HANDLE,
		binding,
		0,
		1,
		static_cast<VkDescriptorType>(type),
		nullptr,
		bufferInfo,
		nullptr
	});
}

void VulkanDescriptor::Writer::add_image_write(const VkDescriptorImageInfo* const imageInfo, const uint16 binding, const Type type) noexcept {
	writes.push_back({
		VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		nullptr,
		VK_NULL_HANDLE,
		binding,
		0,
		1,
		static_cast<VkDescriptorType>(type),
		imageInfo,
		nullptr,
		nullptr
	});
}

// descriptors
VulkanDescriptor::VulkanDescriptor() { }

void VulkanDescriptor::create(const VulkanDevice* const device, const VulkanDescriptorSetLayout* const layout, const VulkanDescriptorPool* const pool, Writer writer) {
	Logger::log_info("Creating Vulkan descriptor sets...");

	// create the descriptor set
	VkDescriptorSetAllocateInfo allocInfo {
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		nullptr,
		pool->get(),
		1,
		layout->get_ptr()
	};

	if (vkAllocateDescriptorSets(device->device(), &allocInfo, &_descriptorSet) != VK_SUCCESS) Logger::log_exception("Failed to allocate descriptor sets");

	for(auto& write : writer.writes) write.dstSet = _descriptorSet;

	vkUpdateDescriptorSets(device->device(), static_cast<uint32>(writer.writes.size()), writer.writes.data(), 0, nullptr);

	Logger::log_info("Succesfully created Vulkan descriptor at ", get_address(this), "!", Logger::end_l());
}

} // namespace lyra

