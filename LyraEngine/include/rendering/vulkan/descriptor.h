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
		struct Data {
			// binding index
			const uint32& binding;
			// type of descriptor to bind
			const int& type;
			// type of shader to bind the descriptor in
			const int& shaderType;
		};

		Builder() = default;

		/**
		 * @brief add a binding to the vector of bindings
		 *
		 * @param newBinding the data for a binding to add
		 */
		void add_binding(const Data& newBinding) noexcept {
			bindings.push_back({
				newBinding.binding,
				static_cast<VkDescriptorType>(newBinding.type),
				1, // newBinding.count,
				static_cast<VkShaderStageFlags>(newBinding.shaderType),
				nullptr
			});
		}

		std::vector<VkDescriptorSetLayoutBinding> bindings;
	};
	
	/**
	 * @brief create the descriptor set layout
	 *
	 * @param builders the builders containing all the creation data
	 */
	DescriptorSetLayout(const std::vector<Builder>& builders);

	/**
	 * @brief destructor of the descriptor set layout
	 */
	~DescriptorSetLayout() noexcept;

	DescriptorSetLayout operator=(const DescriptorSetLayout&) const noexcept = delete;

	/**
	 * @brief get the descriptor set layout
	 *
	 * @return const std::vector<VkDescriptorSetLayout>
	 */
	NODISCARD const std::vector<VkDescriptorSetLayout> get() const noexcept { return m_descriptorSetLayouts; }
	/**
	 * @brief get the descriptor set layout
	 *
	 * @return const VkDescriptorSetLayout
	 */
	NODISCARD const VkDescriptorSetLayout* const data() const noexcept { return m_descriptorSetLayouts.data(); }

private:
	std::vector<VkDescriptorSetLayout> m_descriptorSetLayouts;

	friend class Descriptor;
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
		struct Data {
			// type of the descriptor
			const int& type;
			// number of descriptors needed for this type
			const uint32& size;
		};

		Builder() = default;

		/**
		 * @brief set a struct to define what type and how many types of descriptors a set is going to contain
		 *
		 * @param size data for a pool size
		 */
		void add_pool_size(const Data& size) noexcept {
			poolSizes.push_back({
				static_cast<VkDescriptorType>(size.type),
				size.size
			});
		}

		void add_pool_sizes(const std::vector<Data>& sizes) noexcept {
			for(int i = 0; i < sizes.size(); i++) 
				poolSizes.push_back({
					static_cast<VkDescriptorType>(sizes.at(i).type),
					sizes.at(i).size
				});
		}

		/**	
		 * @brief set the number of maximum possible allocatable sets
		 *
		 * @param m_maxSets the number to set to
		 */
		void set_max_sets(const uint32& m_maxSets) noexcept {
			maxSets = m_maxSets;
		}
		/**
		 * @brief Set the pool flags object
		 *
		 * @param m_poolFlags
		 */
		void set_pool_flags(const VkDescriptorPoolCreateFlags& m_poolFlags) noexcept {
			poolFlags = m_poolFlags;
		}

		std::vector<VkDescriptorPoolSize> poolSizes;
		VkDescriptorPoolCreateFlags poolFlags = 0;
		uint32 maxSets = 1000;
	};

	/**
	 * @brief create a descriptor pool to allocate the descriptor sets
	 *
	 * @param swapchain swapchain
	 */
	DescriptorPool(const Builder& builder);

	/**
	 * @brief destructor of the descriptor pool
	 */
	~DescriptorPool() noexcept;

	DescriptorPool operator=(const DescriptorPool&) const noexcept = delete;

	/**
	 * @brief get the descriptor pool
	 *
	 * @return constexpr VkDescriptorPool
	 */
	NODISCARD constexpr VkDescriptorPool get() const noexcept { return m_descriptorPool; }

private:
	VkDescriptorPool m_descriptorPool;
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
		struct Data {
			// image info
			const VkDescriptorImageInfo& imageInfo;
			// buffer info
			const VkDescriptorBufferInfo& bufferInfo;
			// binding to bind these to
			const uint16& binding;
			// type of shader to bind these to
			const Type& type;
		};

		struct DataI {
			// image info
			const VkDescriptorImageInfo& imageInfo;
			// binding to bind these to
			const uint16& binding;
			// type of shader to bind these to
			const Type& type;
		};

		struct DataB {
			// buffer info
			const VkDescriptorBufferInfo& bufferInfo;
			// binding to bind these to
			const uint16& binding;
			// type of shader to bind these to
			const Type& type;
		};

		Writer() = default;

		/**
		 * @brief add image writes
		 *
		 * @param newWrites image writes to add to the buffer
		 */
		void add_writes(const std::vector<DataI>& newWrites) noexcept {
			for (uint32 i = 0; i < newWrites.size(); i++) {
				writes.push_back({
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
					nullptr,
					VK_NULL_HANDLE,
					newWrites.at(i).binding,
					0,
					1,
					static_cast<VkDescriptorType>(newWrites.at(i).type),
					&newWrites.at(i).imageInfo,
					nullptr,
					nullptr
				});
			}
		}
		/**
		 * @brief add buffer writes
		 *
		 * @param newWrites buffer writes to add to the buffer
		 */
		void add_writes(const std::vector<DataB>& newWrites) noexcept {
			for (uint32 i = 0; i < newWrites.size(); i++) {
				writes.push_back({
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
					nullptr,
					VK_NULL_HANDLE,
					newWrites.at(i).binding,
					0,
					1,
					static_cast<VkDescriptorType>(newWrites.at(i).type),
					nullptr,
					&newWrites.at(i).bufferInfo,
					nullptr
				});
			}
		}
		/**
		 * @brief add writes of both types
		 *
		 * @param newWrites image and buffer writes to add to the buffer
		 */
		void add_writes(const std::vector<Data>& newWrites) noexcept {
			for (const auto& [image_info, buffer_info, binding, type] : newWrites) {
				writes.push_back({
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
					nullptr,
					VK_NULL_HANDLE,
					binding,
					0,
					1,
					static_cast<VkDescriptorType>(type),
					&image_info,
					&buffer_info,
					nullptr
					});
			}
		}

		std::vector<VkWriteDescriptorSet> writes;
	};

	/**
	 * @brief construct new Vulkan descriptors
	 *
	 * @param layout descriptor set layout
	 * @param layoutIndex index of the descriptor set layout
	 * @param pool descriptor pool
	 * @param writer data to be written into the descriptor
	 */
	Descriptor(const DescriptorSetLayout* const layout, const uint32& layoutIndex, const DescriptorPool* const pool, Writer& writer);

	Descriptor operator=(const Descriptor&) const noexcept = delete;

	/**
	 * @brief get the descriptor set
	 *
	 * @return constexpr VkDescriptorSet
	 */
	NODISCARD constexpr VkDescriptorSet get() const noexcept { return m_descriptorSet; }

	/**
	 * @brief get the descriptor set
	 *
	 * @return constexpr const VkDescriptorSet* const
	 */
	NODISCARD constexpr const VkDescriptorSet* const get_ptr() const noexcept { return &m_descriptorSet; }

private:
	VkDescriptorSet m_descriptorSet;
};

} // namespace vulkan

} // namespace lyra
