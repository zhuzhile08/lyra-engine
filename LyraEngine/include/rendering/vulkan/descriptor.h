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

#include <vector>
#include <queue>
#include <core/smart_pointer.h>
#include <core/dynarray.h>
#include <core/basic_pool.h>

#include <core/settings.h>

#include <vulkan/vulkan.h>

namespace lyra {

namespace vulkan {

/**
 * @brief a wrapper class for provinding a better interface for the vulkan descriptor system
 */
class DescriptorSystem {
private:
	/**
	 * @brief wrapper around a Vulkan descriptor set layout
	 *
	 * @brief describe which type of descriptor will be used where
	 */
	class DescriptorSetLayouts {
	public:
		// simplified creation data for a single descriptor set layout
		struct Data {
			const uint32& layoutIndex;
			// type of shader to bind the descriptor in
			const uint32& shaderType;
			// binding index
			const uint32& binding;
			// number of descriptors in the array
			const uint32& arraySize;
			// type of descriptor to bind
			const uint32& type;
		};

		/**
		 * @brief create the descriptor set layout
		 *
		 * @param createInfo creation information
		 */
		DescriptorSetLayouts(const Dynarray<VkDescriptorSetLayoutCreateInfo, Settings::MemConfig::maxDescriptorSetLayouts>& createInfo);

		/**
		 * @brief destructor of the descriptor set layout
		 */
		~DescriptorSetLayouts() noexcept;

		DescriptorSetLayouts operator=(const DescriptorSetLayouts&) const noexcept = delete;

		/**
		 * @brief get the size of the internal dynarray
		 * 
		 * @return const size_t 
		 */
		NODISCARD const size_t size() const noexcept { return size(); }
		/**
		 * @brief cast to the internal dynarray
		 *
		 * @return const Dynarray<VkDescriptorSetLayout, Settings::MemConfig::maxDescriptorSetLayouts>
		 */
		NODISCARD operator const Dynarray<VkDescriptorSetLayout, Settings::MemConfig::maxDescriptorSetLayouts> () const noexcept { return m_descriptorSetLayouts; }
		/**
		 * @brief cast to the data of the internal dynarray
		 *
		 * @return const VkDescriptorSetLayout
		 */
		NODISCARD operator const VkDescriptorSetLayout* const () const noexcept { return m_descriptorSetLayouts.data(); }
		/**
		 * @brief get the descriptor set layout
		 *
		 * @return const lyra::Dynarray<VkDescriptorSetLayout, Settings::MemConfig::maxDescriptorSetLayouts>
		 */
		NODISCARD const Dynarray<VkDescriptorSetLayout, Settings::MemConfig::maxDescriptorSetLayouts> get() const noexcept { return m_descriptorSetLayouts; }
		/**
		 * @brief get the descriptor set layout
		 *
		 * @return const VkDescriptorSetLayout
		 */
		NODISCARD const VkDescriptorSetLayout* const data() const noexcept { return m_descriptorSetLayouts.data(); }

	private:
		Dynarray<VkDescriptorSetLayout, Settings::MemConfig::maxDescriptorSetLayouts> m_descriptorSetLayouts;

		friend class Descriptor;
	};

	/**
	 * @brief wrapper around a Vulkan descriptor pool
	 *
	 * @brief allocates large chunk of memory to allocate descriptor pools
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

		/**
		 * @brief create a descriptor pool to allocate the descriptor sets
		 *
		 * @param createInfo creation information
		 */
		DescriptorPool(const VkDescriptorPoolCreateInfo& createInfo);

		/**
		 * @brief destructor of the descriptor pool
		 */
		~DescriptorPool() noexcept;

		DescriptorPool operator=(const DescriptorPool&) const noexcept = delete;

		/**
		 * @brief cast to the the descriptor pool
		 *
		 * @return constexpr VkDescriptorPool
		 */
		NODISCARD constexpr operator VkDescriptorPool () const noexcept { return m_descriptorPool; }
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
	class DescriptorSet : public BasicPool<DescriptorSet>::ResourceBase {
	public:
		// descriptor types
		enum class Type : uint32 {
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

		/**
		 * @brief construct new Vulkan descriptors
		 *
		 * @param allocInfo descriptor set allocation information
		 * @param pool the pool that this set belongs to
		 */
		DescriptorSet(const VkDescriptorSetAllocateInfo& allocInfo, BasicPool<DescriptorSet>& pool);

		DescriptorSet operator=(const DescriptorSet&) const noexcept = delete;

		/**
		 * @brief add image writes
		 *
		 * @param newWrites image writes to add to the buffer
		 */
		void add_m_writes(const std::vector<DescriptorSet::ImageOnlyData>& newWrites) noexcept {
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
		void add_m_writes(const std::vector<DescriptorSet::BufferOnlyData>& newWrites) noexcept {
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
		void add_m_writes(const std::vector<DescriptorSet::Data>& newWrites) noexcept {
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
		 * @return constexpr VkDescriptorSet
		 */
		NODISCARD constexpr operator VkDescriptorSet () const noexcept { return m_descriptorSet; }
		/**
		 * @brief get the descriptor set
		 *
		 * @return constexpr VkDescriptorSet
		 */
		NODISCARD constexpr VkDescriptorSet get() const noexcept { return m_descriptorSet; }

	private:
		VkDescriptorSet m_descriptorSet;

		std::vector<VkWriteDescriptorSet> m_writes;
	};

public:
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
		void add_binding(const DescriptorSetLayouts::Data& newBinding);

		/**
		 * @brief build the creation info out of the bindings
		 * 
		 * @return const Dynarray<VkDescriptorSetLayoutCreateInfo, 6> 
		 */
		const Dynarray<VkDescriptorSetLayoutCreateInfo, 6> build_create_info() const;

	private:
		Dynarray<std::vector<VkDescriptorSetLayoutBinding>, 6> m_bindings;
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
			 : m_poolSizes(poolBuilder.m_poolSizes), m_poolFlags(poolBuilder.m_poolFlags), m_maxSets(poolBuilder.m_maxSets) { }
		
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

			m_maxSets += Settings::MemConfig::maxDescriptorTypePerPool * newSize.multiplier;
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
		 * @brief Set the pool flags object
		 *
		 * @param poolFlags pool creation flags
		 */
		void set_pool_flags(const VkDescriptorPoolCreateFlags& poolFlags) noexcept {
			m_poolFlags = poolFlags;
		}

		/**
		 * @brief build the creation info out of the bindings
		 * 
		 * @return const VkDescriptorPoolCreateInfo
		 */
		const VkDescriptorPoolCreateInfo build_create_info() const  {
			// return the create info
			return VkDescriptorPoolCreateInfo {
				VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
				nullptr,
				m_poolFlags,
				m_maxSets,
				static_cast<uint32>(m_poolSizes.size()),
				m_poolSizes.data()
			};
		}

	private:
		std::vector<VkDescriptorPoolSize> m_poolSizes;
		VkDescriptorPoolCreateFlags m_poolFlags = 0;
		uint32 m_maxSets;
	};

	/**
	 * @brief create a descriptor set layout and return a pointer
	 * 
	 * @param layoutBuilder layout builder
	 * 
	 * @return lyra::vulkan::DescriptorSystem::DescriptorSetLayouts* const 
	 */
	DescriptorSetLayouts* const create_descriptor_set_layout(const LayoutBuilder& layoutBuilder, const PoolBuilder& poolBuilder) {
		m_layouts = m_layouts.create(layoutBuilder.build_create_info());
		m_poolBuilder = m_poolBuilder.create(poolBuilder);
	}

	NODISCARD DescriptorSet& get_unused_set(const size_t& layoutIndex);

	/**
	 * @brief get the descriptor set layout
	 * 
	 * @return const lyra::vulkan::DescriptorSystem::DescriptorSetLayouts&
	 */
	NODISCARD const DescriptorSetLayouts& layouts() const noexcept { return *m_layouts; }
	/**
	 * @brief get the descriptor pools
	 * 
	 * @return const std::vector<lyra::vulkan::DescriptorSystem::DescriptorPool> 
	 */
	NODISCARD const std::vector<DescriptorPool> pools() const noexcept { return m_pools; }
	/**
	 * @brief get the descriptor sets
	 * 
	 * @return const Array<BasicPool<lyra::vulkan::DescriptorSystem::DescriptorSet>, Settings::MemConfig::maxDescriptorSetLayouts>
	 */
	NODISCARD const Array<BasicPool<DescriptorSet>, Settings::MemConfig::maxDescriptorSetLayouts> sets() const noexcept { return m_sets; }
	/**
	 * @brief get the descriptor sets
	 * 
	 * @param index index of the basic pool
	 * 
	 * @return const Array<BasicPool<lyra::vulkan::DescriptorSystem::DescriptorSet>, Settings::MemConfig::maxDescriptorSetLayouts>
	 */
	NODISCARD const BasicPool<DescriptorSet> sets(const size_t& index) const noexcept { return m_sets[index]; }

private:
	SmartPointer<DescriptorSetLayouts> m_layouts;
	std::vector<DescriptorPool> m_pools;
	Array<BasicPool<DescriptorSet>, Settings::MemConfig::maxDescriptorSetLayouts> m_sets;

	SmartPointer<PoolBuilder> m_poolBuilder;

	/**
	 * @brief create a descriptor pool and its respecting descriptor sets
	 * 
	 * @param layoutIndex index of the layout to create it's descriptor pools with
	 */
	void create_descriptor_pool(const uint32& layoutIndex);

};

} // namespace vulkan

} // namespace lyra
