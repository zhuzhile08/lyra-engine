/*************************
 * @file descriptor.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief wrapper around the Vulkan destriptor sets
 *
 * @date 2022-02-05
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <core/defines.h>
#include <core/rendering/vulkan/swapchain.h>
#include <core/logger.h>

#include <vector>

#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief wrapper around a Vulkan descriptor set layout
 *
 * @brief describe which type of descriptor will be used where
 */
class VulkanDescriptorSetLayout {
public:
	/**
	 * @brief a builder to make the creation of the descriptor layout easier
	 */
	struct Builder {
		Builder();

		/**
		 * @brief destructor of the builder
		 */
		virtual ~Builder() noexcept;

		/**
		 * @brief add a binding to the vector of bindings
		 *
		 * @param binding at which index the shader expects the descriptor
		 * @param type type of descriptor that will be used
		 * @param stage which shader can access the descriptor
		 * @param count numbers of descriptors in the binding
		 */
		void add_binding(const uint32 binding, const VkDescriptorType type, const VkShaderStageFlags stage, const uint32 count = 1) noexcept;

		std::vector<VkDescriptorSetLayoutBinding> bindings;
	};

	VulkanDescriptorSetLayout();

	/**
	 * @brief destructor of the descriptor set layout
	 */
	virtual ~VulkanDescriptorSetLayout() noexcept;

	/**
	 * @brief destroy the descriptor set layout
	 */
	void destroy() noexcept;

	VulkanDescriptorSetLayout operator=(const VulkanDescriptorSetLayout&) const noexcept = delete;

	/**
	 * @brief create the descriptor set layout
	 *
	 * @param device device
	 * @param builder the builder containing all the creation data
	 */
	void create(const VulkanDevice* const device, const Builder builder);
	/**
	 * @brief get the descriptor set layout
	 *
	 * @return const VkDescriptorSetLayout
	 */
	[[nodiscard]] const VkDescriptorSetLayout get() const noexcept { return _descriptorSetLayout; }
	/**
	 * @brief get the descriptor set layout as pointers
	 *
	 * @return const VkDescriptorSetLayout*
	 */
	[[nodiscard]] const VkDescriptorSetLayout* get_ptr() const noexcept { return &_descriptorSetLayout; }

private:
	VkDescriptorSetLayout _descriptorSetLayout = VK_NULL_HANDLE;

	const VulkanDevice* device;
};

/**
 * @brief wrapper around a Vulkan descriptor pool
 *
 * @brief allocates large chunk of memory to allocate descriptor pools
 */
class VulkanDescriptorPool {
public:
	/**
	 * @brief a builder to make the creation of the descriptor pool easier
	 */
	struct Builder {
		Builder();

		/**
		 * @brief destructor of the builder
		 */
		virtual ~Builder() noexcept;

		/**
		 * @brief set a struct to define wwhat type and how many types of descriptors a set is going to contain
		 *
		 * @param sizes all the sizes and types of descriptors contained inside of a pair inside of an vector. Pair consists of a const VKDescriptorType and a const uint32
		 */
		void add_pool_sizes(std::vector<std::pair<const VkDescriptorType, const uint32>> sizes) noexcept;
		/**
		 * @brief set the number of maximum possible allocatable sets
		 *
		 * @param _maxSets the number to set to
		 */
		void set_max_sets(const uint32 _maxSets) noexcept;

		/**
		 * @brief Set the pool flags object
		 *
		 * @param _poolFlags
		 */
		void set_pool_flags(const VkDescriptorPoolCreateFlags _poolFlags) noexcept;

		std::vector<VkDescriptorPoolSize> poolSizes;
		VkDescriptorPoolCreateFlags poolFlags = 0;
		uint32 maxSets = 1000;
	};

	VulkanDescriptorPool();

	/**
	 * @brief destructor of the descriptor pool
	 */
	virtual ~VulkanDescriptorPool() noexcept;

	/**
	 * @brief destroy the descriptor pool
	 */
	void destroy() noexcept;

	VulkanDescriptorPool operator=(const VulkanDescriptorPool&) const noexcept = delete;

	/**
	 * @brief create a descriptor pool to allocate the descriptor sets
	 *
	 * @param device device
	 * @param swapchain swapchain
	 */
	void create(const VulkanDevice* const device, const Builder builder);

	/**
	 * @brief get the descriptor pool
	 *
	 * @return const VkDescriptorPool
	 */
	[[nodiscard]] const VkDescriptorPool get() const noexcept { return _descriptorPool; }

private:
	VkDescriptorPool _descriptorPool = VK_NULL_HANDLE;

	const VulkanDevice* device;
};

/**
 * @brief wrapper around the Vulkan descriptor set
 */
class VulkanDescriptor {
public:
	/**
	 * @brief struct to configure what will be written into the descriptor sets
	 */
	struct Writer {
		Writer();

		/**
		 * @brief destructor of the wrter
		 */
		virtual ~Writer() noexcept;

		/**
		 * @brief add a setting for buffers
		 *
		 * @param bufferInfo information about the buffer
		 * @param binding at which position it will be entered into the shader
		 * @param type type of the descriptor set
		 */
		void add_buffer_write(const VkDescriptorBufferInfo* const bufferInfo, const uint16 binding = 0, const VkDescriptorType type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER) noexcept;
		/**
		 * @brief add a setting for images
		 *
		 * @param imageInfo information about the image
		 * @param binding at which position it will be entered into the shader
		 * @param type type of the descriptor set
		 */
		void add_image_write(const VkDescriptorImageInfo* const imageInfo, const uint16 binding = 1, const VkDescriptorType type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) noexcept;

		std::vector<VkWriteDescriptorSet> writes;
	};

	VulkanDescriptor();

	VulkanDescriptor operator=(const VulkanDescriptor&) const noexcept = delete;

	/**
	 * @brief construct new Vulkan descriptors
	 *
	 * @param device the device
	 * @param layout descriptor set layout
	 * @param pool descriptor pool
	 * @param writer data to be written into the descriptor
	 */
	void create(const VulkanDevice* const device, const VulkanDescriptorSetLayout* const layout, const VulkanDescriptorPool* const pool, Writer writer);

	/**
	 * @brief get the descriptor set
	 *
	 * @return const VkDescriptorSet
	 */
	[[nodiscard]] const VkDescriptorSet get() const noexcept { return _descriptorSet; }

	/**
	 * @brief get the descriptor set
	 *
	 * @return const VkDescriptorSet*
	 */
	[[nodiscard]] const VkDescriptorSet* get_ptr() const noexcept { return &_descriptorSet; }

private:
	VkDescriptorSet _descriptorSet = VK_NULL_HANDLE;
};

} // namespace lyra
