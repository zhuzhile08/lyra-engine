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

#include <lyra.h>

#include <algorithm>
#include <vector>
#include <queue>
#include <core/smart_pointer.h>
#include <core/dynarray.h>
#include <core/resource_pool.h>
#include <core/function_pointer.h>

#include <core/settings.h>

#include <vulkan/vulkan.h>
#include <rendering/vulkan/vulkan_raii.h>

namespace lyra {

namespace vulkan {

/**
 * @brief a wrapper class for provinding a better interface for the vulkan descriptor system
 */
class DescriptorSystem {
private:
	/**
	 * @brief wrapper around a Vulkan descriptor set layout
	 */
	class DescriptorSetLayout {
	public:
		// simplified creation data for a single descriptor set layout
		struct Data {
			// type of shader to bind the descriptor in
			const uint32& shaderType;
			// binding index
			const uint32& binding;
			// number of descriptors in the array
			const uint32& arraySize;
			// type of descriptor to bind
			const uint32& type;
		};

		constexpr DescriptorSetLayout() = default;
		/**
		 * @brief create the descriptor set layout
		 *
		 * @param createInfo creation information
		 */
		DescriptorSetLayout(const VkDescriptorSetLayoutCreateInfo& createInfo);

		DescriptorSetLayout(DescriptorSetLayout&&) = default;
		DescriptorSetLayout& operator=(DescriptorSetLayout&&) = default;

		/**
		 * @brief cast to the internal descriptor set layout
		 *
		 * @return lyra::vulkan::vk::DescriptorSetLayout&
		 */
		NODISCARD constexpr operator vk::DescriptorSetLayout& () noexcept { return m_descriptorSetLayout; }
		/**
		 * @brief cast to the internal descriptor set layout
		 *
		 * @return const lyra::vulkan::vk::DescriptorSetLayout&
		 */
		NODISCARD constexpr operator const vk::DescriptorSetLayout& () const noexcept { return m_descriptorSetLayout; }
		/**
		 * @brief get the descriptor set layout
		 *
		 * @return lyra::vulkan::vk::DescriptorSetLayout&
		 */
		NODISCARD constexpr vk::DescriptorSetLayout& get() noexcept { return m_descriptorSetLayout; }
		/**
		 * @brief get the descriptor set layout
		 *
		 * @return const lyra::vulkan::vk::DescriptorSetLayout&
		 */
		NODISCARD constexpr const vk::DescriptorSetLayout& get() const noexcept { return m_descriptorSetLayout; }

	private:
		vk::DescriptorSetLayout m_descriptorSetLayout;

		friend class Descriptor;
	};

	/**
	 * @brief wrapper around a Vulkan descriptor pool
	 */
	class DescriptorPool {
	public:
		// simplified size data
		struct Size {
			// type of descriptor
			const uint32& type;
			// multiplier for the descriptor allocation count
			const uint32& multiplier;
		};

		constexpr DescriptorPool() = default;
		/**
		 * @brief create a descriptor pool to allocate the descriptor sets
		 *
		 * @param createInfo creation information
		 */
		DescriptorPool(const VkDescriptorPoolCreateInfo& createInfo);

		DescriptorPool(DescriptorPool&&) = default;
		DescriptorPool& operator=(DescriptorPool&&) = default;

		/**
		 * @brief get the capacity of the pool
		 * 
		 * @return constexpr size_t
		 */
		NODISCARD constexpr size_t capacity() const noexcept { return m_capacity; }
		/**
		 * @brief cast to the the descriptor pool
		 *
		 * @return constexpr lyra::vulkan::vk::DescriptorPool&
		 */
		NODISCARD constexpr operator vk::DescriptorPool& () noexcept { return m_descriptorPool; }
		/**
		 * @brief cast to the the descriptor pool
		 *
		 * @return constexpr const lyra::vulkan::vk::DescriptorPool&
		 */
		NODISCARD constexpr operator const vk::DescriptorPool& () const noexcept { return m_descriptorPool; }
		/**
		 * @brief get the descriptor pool
		 *
		 * @return constexpr lyra::vulkan::vk::DescriptorPool&
		 */
		NODISCARD constexpr vk::DescriptorPool& get() noexcept { return m_descriptorPool; }
		/**
		 * @brief get the descriptor pool
		 *
		 * @return constexpr const lyra::vulkan::vk::DescriptorPool&
		 */
		NODISCARD constexpr const vk::DescriptorPool& get() const noexcept { return m_descriptorPool; }

	private:
		vk::DescriptorPool m_descriptorPool;
		size_t m_capacity;
	};

public:
	/**
	 * @brief wrapper around the Vulkan descriptor set
	 */
	class DescriptorSet {
	public:
		// descriptor types
		enum Type : uint32 {
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
		}; 
		
		// creation data for a single descriptor with both image and buffer information
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

		// creation data for a single descriptor with only image information
		struct ImageOnlyData {
			// image info
			const VkDescriptorImageInfo& imageInfo;
			// binding to bind these to
			const uint16& binding;
			// type of shader to bind these to
			const Type& type;
		};

		// creation data for a single descriptor with only buffer information
		struct BufferOnlyData {
			// buffer info
			const VkDescriptorBufferInfo& bufferInfo;
			// binding to bind these to
			const uint16& binding;
			// type of shader to bind these to
			const Type& type;
		};

		DescriptorSet() = default;
		/**
		 * @brief construct a new descriptor set wrapper
		 *
		 * @param allocInfo descriptor set allocation information
		 */
		DescriptorSet(const VkDescriptorSetAllocateInfo& allocInfo);
		/**
		 * @brief move constructor of the descriptor set wrapper
		 * 
		 * @param movable descriptor set wrapper rvalue
		 */
		DescriptorSet(DescriptorSet&& movable) : 
			m_descriptorSet(std::exchange(movable.m_descriptorSet, VkDescriptorSet { } )),
			m_writes(movable.m_writes) { }
		/**
		 * @brief move operator of the descriptor set wrapper
		 * 
		 * @param movable descriptor set wrapper rvalue
		 * 
		 * @return DescriptorSet& 
		 */
		constexpr DescriptorSet& operator=(DescriptorSet&& movable) {
			if (movable.m_descriptorSet != this->m_descriptorSet) {
				m_descriptorSet = std::exchange(movable.m_descriptorSet, VkDescriptorSet { } );
				m_writes = movable.m_writes;
			}
			return *this;
		}

		/**
		 * @brief add image writes
		 *
		 * @param newWrites image writes to add to the buffer
		 */
		void add_writes(const std::vector<DescriptorSet::ImageOnlyData>& newWrites) noexcept {
			for (uint32 i = 0; i < newWrites.size(); i++) {
				m_writes.push_back({
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
					nullptr,
					m_descriptorSet,
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
		void add_writes(const std::vector<DescriptorSet::BufferOnlyData>& newWrites) noexcept {
			for (uint32 i = 0; i < newWrites.size(); i++) {
				m_writes.push_back({
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
					nullptr,
					m_descriptorSet,
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
		 * @brief add m_writes of both types
		 *
		 * @param newWrites image and buffer m_writes to add to the buffer
		 */
		void add_writes(const std::vector<DescriptorSet::Data>& newWrites) noexcept {
			for (const auto& [image_info, buffer_info, binding, type] : newWrites) {
				m_writes.push_back({
					VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
					nullptr,
					m_descriptorSet,
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

		/**
		 * @brief write the updates into the descriptor set
		 */
		void update() const noexcept;

		/**
		 * @brief cast to the descriptor set
		 *
		 * @return const VkDescriptorSet&
		 */
		NODISCARD constexpr operator const VkDescriptorSet& () const noexcept { return m_descriptorSet; }
		/**
		 * @brief get the descriptor set
		 *
		 * @return const VkDescriptorSet&
		 */
		NODISCARD constexpr const VkDescriptorSet& get() const noexcept { return m_descriptorSet; }

	private:
		VkDescriptorSet m_descriptorSet;

		std::vector<VkWriteDescriptorSet> m_writes;
	};

	/**
	 * @brief descriptor resource type, annoying to type every time
	 */
	typedef ResourcePool<DescriptorSet>::resource_container_type DescriptorSetResource;

	/**
	 * @brief a builder to make the creation of the descriptor layout easier
	 */
	class LayoutBuilder {
	public:
		/**
		 * @brief add a binding to the vector of bindings
		 *
		 * @param newBinding the data for a binding to add
		 */
		void add_binding(const DescriptorSetLayout::Data& newBinding);

		/**
		 * @brief build the creation info out of the bindings
		 * 
		 * @return VkDescriptorSetLayoutCreateInfo
		 */
		VkDescriptorSetLayoutCreateInfo build_create_info() const {
			return VkDescriptorSetLayoutCreateInfo {
				VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
				nullptr,
				0,
				static_cast<uint32>(m_bindings.size()),
				m_bindings.data()
			};
		}

	private:
		std::vector<VkDescriptorSetLayoutBinding> m_bindings;
	};

	/**
	 * @brief a builder to make the creation of the descriptor pool easier
	 */
	class PoolBuilder {
	public:
		PoolBuilder() = default;
		/**
		 * @brief construct a pool builder from another pool builder
		 * 
		 * @param poolBuilder the other pool builder
		 */
		PoolBuilder(const PoolBuilder& poolBuilder)
			 : m_poolSizes(poolBuilder.m_poolSizes), m_poolFlags(poolBuilder.m_poolFlags) { } // , m_maxSets(poolBuilder.m_maxSets) { }
		
		/**
		 * @brief add one descriptor type to the descriptor pool
		 *
		 * @param newSize single simplified pool size structure
		 */
		void add_pool_size(const DescriptorPool::Size& newSize) noexcept {
			// add the size according to the type
			m_poolSizes.push_back({
				static_cast<VkDescriptorType>(newSize.type),
				uint32(newSize.multiplier * Settings::MemConfig::maxDescriptorTypePerPool)
			});

			// m_maxSets = Settings::MemConfig::maxDescriptorTypePerPool * newSize.multiplier;
		}
		/**
		 * @brief add multiple types to the descriptor pool
		 * 
		 * @param newSzes multiple simplified pool size structures
		 */
		void add_pool_sizes(const std::vector<DescriptorPool::Size>& newSizes) noexcept {
			// loop through all the types and create their respective sizes
			for (const auto& newSize : newSizes) add_pool_size(newSize);
		}

		/**
		 * @brief set the pool flags
		 *
		 * @param poolFlags pool creation flags
		 */
		void set_pool_flags(const VkDescriptorPoolCreateFlags& poolFlags) noexcept {
			m_poolFlags = poolFlags;
		}

		/**
		 * @brief build the creation info out of the bindings
		 * 
		 * @return constexpr VkDescriptorPoolCreateInfo
		 */
		constexpr VkDescriptorPoolCreateInfo build_create_info() const  {
			// return the create info
			return VkDescriptorPoolCreateInfo {
				VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
				nullptr,
				m_poolFlags,
				Settings::MemConfig::maxDescriptorTypePerPool,
				// m_maxSets,
				static_cast<uint32>(m_poolSizes.size()),
				m_poolSizes.data()
			};
		}

	private:
		std::vector<VkDescriptorPoolSize> m_poolSizes;
		VkDescriptorPoolCreateFlags m_poolFlags = 0;
		// uint32 m_maxSets;
	};

	DescriptorSystem() = default;
	/**
	 * @brief construct a new descriptor system
	 * 
	 * @param layoutBuilder descriptor set layout builder to build the layout with
	 * @param poolBuilder descriptor pool builder to build all future pools in the builder with
	 */
	DescriptorSystem(const LayoutBuilder& layoutBuilder, const PoolBuilder& poolBuilder) : m_poolBuilder(poolBuilder) {
		m_layout = DescriptorSetLayout(layoutBuilder.build_create_info());
	}
	DescriptorSystem(DescriptorSystem&&) = default;
	DescriptorSystem& operator=(DescriptorSystem&&) = default;
	
	/**
	 * @brief get an unused descriptor set
	 * 
	 * @return lyra::vulkan::DescriptorSystem::DescriptorSetResource
	 */
	NODISCARD DescriptorSetResource get_unused_set();

	/**
	 * @brief get the descriptor set layout
	 * 
	 * @return const lyra::vulkan::DescriptorSystem::DescriptorSetLayout&
	 */
	NODISCARD constexpr const DescriptorSetLayout& layout() const noexcept { return m_layout; }
	/**
	 * @brief get the descriptor pools
	 * 
	 * @return const lyra::vulkan::DescriptorSystem::DescriptorPool&
	 */
	NODISCARD constexpr const std::vector<DescriptorPool>& pools() const noexcept { return m_pools; }

private:
	DescriptorSetLayout m_layout;
	std::vector<DescriptorPool> m_pools;
	ResourcePool<DescriptorSet> m_sets;

	PoolBuilder m_poolBuilder;

	/**
	 * @brief create a descriptor pool and its respecting descriptor sets
	 */
	void create_descriptor_pool();

	friend class lyra::gui::GUIRenderer;
	friend class Pipeline;	
};

} // namespace vulkan

} // namespace lyra
