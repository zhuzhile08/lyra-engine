/*************************
 * @file descriptor.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief wrapper around the Vulkan destriptor sets
 * 
 * @date 2022-02-05
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <core/defines.h>
#include <rendering/vulkan/GPU_buffer.h>
#include <rendering/vulkan/swapchain.h>
#include <rendering/camera.h>
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
		 * @brief add a binding to the vector of bindings
		 * 
		 * @param binding at which index the shader expects the descriptor
		 * @param type type of descriptor that will be used
		 * @param stage which shader can access the descriptor
		 * @param count numbers of descriptors in the binding
		 */
		void 										add_binding(const uint32 binding, const VkDescriptorType type, const VkShaderStageFlags stage, const uint32 count = 1);

		std::vector<VkDescriptorSetLayoutBinding> 	bindings;
	};

	VulkanDescriptorSetLayout();

	/**
	 * @brief destroy the descriptor set layout
	 */
	void 							destroy();

	/**
	 * @brief create the descriptor set layout
	 * 
	 * @param device device
	 * @param type types of descriptors used ored together
	 * @param typeCount number of types
	 * @param flags shader stage flags ored together
	 */
	void 							create(VulkanDevice device, const Builder builder);
	/**
	 * @brief get the descriptor set layout
	 * 
	 * @return VkDescriptorSetLayout 
	 */
	VkDescriptorSetLayout 			get() const;
	/**
	 * @brief get the descriptor set layout as pointers
	 * 
	 * @return VkDescriptorSetLayout 
	 */
	const VkDescriptorSetLayout* 	get_ptr() const;

private:
	VkDescriptorSetLayout	descriptorSetLayout;

	VulkanDevice*			device;
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
		 * @brief set a struct to define wwhat type and how many types of descriptors a set is going to contain
		 * 
		 * @param type type of descriptor
		 * @param count number of descriptors
		 */
		void 								add_pool_sizes(const VkDescriptorType type, const uint32_t count);
		/**
		 * @brief set the number of maximum possible allocatable sets
		 * 
		 * @param _maxSets the number to set to
		 */
		void 								set_max_sets(const uint32 _maxSets);
		
		/**
		 * @brief Set the pool flags object
		 * 
		 * @param _poolFlags 
		 */
		void 								set_pool_flags(const VkDescriptorPoolCreateFlags _poolFlags);

		std::vector<VkDescriptorPoolSize> 	poolSizes;
		VkDescriptorPoolCreateFlags			poolFlags = 0;
		uint32								maxSets = 1000;
	};

	VulkanDescriptorPool();

	/**
	 * @brief destroy the descriptor pool
	 */
	void 				destroy();

	/**
	 * @brief create a descriptor pool to allocate the descriptor sets
	 * 
	 * @param device device
	 * @param swapchain swapchain 
	 */
	void				create(VulkanDevice device, const Builder builder);

	/**
	 * @brief get the descriptor pool
	 * 
	 * @return VkDescriptorPool 
	 */
	VkDescriptorPool	get() const;

private:
	VkDescriptorPool 	descriptorPool;

	VulkanDevice*		device;
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
	};

	VulkanDescriptor();

	/**
	 * @brief destroy the Vulkan descriptors
	 * 
	 * @param device device
	 * @param swapchain swapchain
	 */
	void							destroy();

	/**
	 * @brief construct new Vulkan descriptors
	 * 
	 * @param device the device
	 * @param size number of descriptors to create
	 * @param layout descriptor set layout
	 * @param pool descriptor pool
	 */
	void 							create(VulkanDevice device, const uint16 size, const VulkanDescriptorSetLayout layout, const VulkanDescriptorPool pool, const Writer writer);

	/**
	 * @brief get the descriptor set
	 * 
	 * @return std::vector<VkDescriptorSet>
	 */
	std::vector<VkDescriptorSet>	get() const;

private:
	std::vector<VkDescriptorSet> 	descriptorSets;

	VulkanDevice* 					device;
};

} // namespace lyra
