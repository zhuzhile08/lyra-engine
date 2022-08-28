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

#include <core/decl.h>
#include <core/application.h>

#include <array>
#include <vector>

#include <vulkan/vulkan.h>

namespace lyra {

namespace vulkan {

/**
 * @brief wrapper around a Vulkan descriptor set layout
 *
 * @brief describe which type of descriptor will be used where
 */
class DescriptorSetLayout {
public:
	/**
	 * @brief a builder to make the creation of the descriptor layout easier
	 */
	struct Builder {
		Builder() { }

		/**
		 * @brief add a binding to the vector of bindings
		 *
		 * @param newBindings a vector with the data for a binding. Consists of the binding index, the type of descriptor to bind, the shader behind that descriptor and the number bindings of that type
		 */
		void add_bindings(std::vector<std::tuple<const uint32, const int, const int, const uint32>> newBindings) noexcept {
			for (const auto& [binding, type, shaderType, count] : newBindings) {
				bindings.push_back({
					binding,
					static_cast<VkDescriptorType>(type),
					count,
					static_cast<VkShaderStageFlags>(shaderType),
					nullptr
				});
			}
		}

		std::vector<VkDescriptorSetLayoutBinding> bindings;
	};

	DescriptorSetLayout() { }
	/**
	 * @brief create the descriptor set layout
	 *
	 * @param builder the builder containing all the creation data
	 */
	DescriptorSetLayout(const Builder builder);

	/**
	 * @brief destructor of the descriptor set layout
	 */
	~DescriptorSetLayout() noexcept;
	/**
	 * @brief destroy the descriptor set layout
	 */
	void destroy() noexcept {
		this->~DescriptorSetLayout();
	}

	DescriptorSetLayout operator=(const DescriptorSetLayout&) const noexcept = delete;

	/**
	 * @brief get the descriptor set layout
	 *
	 * @return const VkDescriptorSetLayout&
	 */
	NODISCARD const VkDescriptorSetLayout& get() const noexcept { return m_descriptorSetLayout; }
	/**
	 * @brief get the descriptor set layout as pointers
	 *
	 * @return const VkDescriptorSetLayout* const
	 */
	NODISCARD const VkDescriptorSetLayout* const get_ptr() const noexcept { return &m_descriptorSetLayout; }

private:
	VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
};

/**
 * @brief wrapper around a Vulkan descriptor pool
 *
 * @brief allocates large chunk of memory to allocate descriptor pools
 */
class DescriptorPool {
public:
	/**
	 * @brief a builder to make the creation of the descriptor pool easier
	 */
	struct Builder {
		Builder() { };

		/**
		 * @brief set a struct to define wwhat type and how many types of descriptors a set is going to contain
		 *
		 * @param sizes all the sizes and types of descriptors contained inside of a pair inside of an vector. Pair consists of the type of the descriptor and the number of descriptors for that type
		 */
		void add_pool_sizes(std::vector<std::pair<const int, const uint32>> sizes) noexcept {
			for (const auto& [type, size] : sizes) {
				poolSizes.push_back({
					static_cast<VkDescriptorType>(type),
					size
				});
			}
		}
		/**
		 * @brief set the number of maximum possible allocatable sets
		 *
		 * @param m_maxSets the number to set to
		 */
		void set_max_sets(const uint32 m_maxSets) noexcept {
			maxSets = m_maxSets;
		}
		/**
		 * @brief Set the pool flags object
		 *
		 * @param m_poolFlags
		 */
		void set_pool_flags(const VkDescriptorPoolCreateFlags m_poolFlags) noexcept {
			poolFlags = m_poolFlags;
		}

		std::vector<VkDescriptorPoolSize> poolSizes;
		VkDescriptorPoolCreateFlags poolFlags = 0;
		uint32 maxSets = 1000;
	};

	DescriptorPool() { }
	/**
	 * @brief create a descriptor pool to allocate the descriptor sets
	 *
	 * @param swapchain swapchain
	 */
	DescriptorPool(const Builder builder);

	/**
	 * @brief destructor of the descriptor pool
	 */
	~DescriptorPool() noexcept;
	/**
	 * @brief destroy the descriptor pool
	 */
	void destroy() noexcept {
		this->~DescriptorPool();
	}

	DescriptorPool operator=(const DescriptorPool&) const noexcept = delete;

	/**
	 * @brief get the descriptor pool
	 *
	 * @return const VkDescriptorPool&
	 */
	NODISCARD const VkDescriptorPool& get() const noexcept { return m_descriptorPool; }

private:
	VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
};

/**
 * @brief wrapper around the Vulkan descriptor set
 */
class Descriptor {
public:
	// descriptor types
	enum Type : int {
		// sampler
		TYPE_SAMPLER = 0,
		// image sampler
		TYPE_IMAGE_SAMPLER = 1,
		// sampled image
		TYPE_SAMPLED_IMAGE = 2,
		// image used for storage
		TYPE_STORAGE_IMAGE = 3,
		// texel uniform buffer
		TYPE_UNIFORM_TEXEL_BUFFER = 4,
		// texel storage buffer
		TYPE_STORAGE_TEXEL_BUFFER = 5,
		// uniform buffer
		TYPE_UNIFORM_BUFFER = 6,
		// storage buffer
		TYPE_STORAGE_BUFFER = 7,
		// dynamic uniform buffer
		TYPE_UNIFORM_BUFFER_DYNAMIC = 8,
		// dynamic storage buffer
		TYPE_STORAGE_BUFFER_DYNAMIC = 9,
		// image
		TYPE_INPUT_ATTACHMENT = 10,
		// uniform buffer, but inline
		TYPE_INLINE_UNIFORM_BLOCK = 1000138000,
		// mutables
		TYPE_MUTABLE_VALVE = 1000351000,
	}; // strongy typed enums suck and you know it

	/**
	 * @brief struct to configure what will be written into the descriptor sets
	 */
	struct Writer {
		Writer() { }

		/**
		 * @brief add a write
		 *
		 * @param newWrites Writes to add to the buffer. Consists of a image information, a buffer information, the binding and a type
		 */
		void add_writes(std::vector<std::tuple<const VkDescriptorImageInfo*, const VkDescriptorBufferInfo*, const uint16, const Type>> newWrites) noexcept {
			for (const auto &[image_info, buffer_info, binding, type] : newWrites) {
				writes.push_back({
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
					nullptr,
					VK_NULL_HANDLE,
					binding,
					0,
					1,
					static_cast<VkDescriptorType>(type),
					image_info,
					buffer_info,
					nullptr
				});
			}
		}

		std::vector<VkWriteDescriptorSet> writes;
	};

	Descriptor() { }
	/**
	 * @brief construct new Vulkan descriptors
	 *
	 * @param layout descriptor set layout
	 * @param pool descriptor pool
	 * @param writer data to be written into the descriptor
	 */
	Descriptor(const DescriptorSetLayout* const layout, const DescriptorPool* const pool, Writer writer);

	Descriptor operator=(const Descriptor&) const noexcept = delete;

	/**
	 * @brief get the descriptor set
	 *
	 * @return const VkDescriptorSet&
	 */
	NODISCARD const VkDescriptorSet& get() const noexcept { return m_descriptorSet; }

	/**
	 * @brief get the descriptor set
	 *
	 * @return const VkDescriptorSet* const
	 */
	NODISCARD const VkDescriptorSet* const get_ptr() const noexcept { return &m_descriptorSet; }

private:
	VkDescriptorSet m_descriptorSet = VK_NULL_HANDLE;
};

} // namespace vulkan

} // namespace lyra
