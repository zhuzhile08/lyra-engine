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
#include <core/logger.h>
#include <core/rendering/vulkan/vulkan_shader.h>
#include <math/math.h>
#include <lyra.h>

#include <array>
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
		Builder() { }

		/**
		 * @brief destructor of the builder
		 */
		~Builder() noexcept {
			bindings.clear();
		}

		/**
		 * @brief add a binding to the vector of bindings
		 *
		 * @param newBindings a vector with the data for a binding. Consists of the binding index, the type of descriptor to bind, the shader behind that descriptor and the number bindings of that type
		 */
		void add_binding(std::vector<std::tuple<const uint32, const int, const VulkanShader::Type, const uint32>> newBindings) noexcept {
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

	VulkanDescriptorSetLayout() { }

	/**
	 * @brief destructor of the descriptor set layout
	 */
	~VulkanDescriptorSetLayout() noexcept {
		vkDestroyDescriptorSetLayout(Application::context()->device()->device(), _descriptorSetLayout, nullptr);

		Logger::log_info("Successfully destroyed Vulkan descriptor set layout!");
	}

	/**
	 * @brief destroy the descriptor set layout
	 */
	void destroy() noexcept {
		this->~VulkanDescriptorSetLayout();
	}

	VulkanDescriptorSetLayout operator=(const VulkanDescriptorSetLayout&) const noexcept = delete;

	/**
	 * @brief create the descriptor set layout
	 *
	 * @param builder the builder containing all the creation data
	 */
	void create(const Builder builder);
	/**
	 * @brief get the descriptor set layout
	 *
	 * @return const VkDescriptorSetLayout&
	 */
	[[nodiscard]] const VkDescriptorSetLayout& get() const noexcept { return _descriptorSetLayout; }
	/**
	 * @brief get the descriptor set layout as pointers
	 *
	 * @return const VkDescriptorSetLayout* const
	 */
	[[nodiscard]] const VkDescriptorSetLayout* const get_ptr() const noexcept { return &_descriptorSetLayout; }

private:
	VkDescriptorSetLayout _descriptorSetLayout = VK_NULL_HANDLE;
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
		Builder() { };

		/**
		 * @brief destructor of the builder
		 */
		virtual ~Builder() noexcept {
			poolSizes.clear();
		}

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
		 * @param _maxSets the number to set to
		 */
		void set_max_sets(const uint32 _maxSets) noexcept {
			maxSets = _maxSets;
		}
		/**
		 * @brief Set the pool flags object
		 *
		 * @param _poolFlags
		 */
		void set_pool_flags(const VkDescriptorPoolCreateFlags _poolFlags) noexcept {
			poolFlags = _poolFlags;
		}

		std::vector<VkDescriptorPoolSize> poolSizes;
		VkDescriptorPoolCreateFlags poolFlags = 0;
		uint32 maxSets = 1000;
	};

	VulkanDescriptorPool();

	/**
	 * @brief destructor of the descriptor pool
	 */
	~VulkanDescriptorPool() noexcept {
		vkDestroyDescriptorPool(Application::context()->device()->device(), _descriptorPool, nullptr);

		Logger::log_info("Successfully destroyed Vulkan descriptor pool!");
	}

	/**
	 * @brief destroy the descriptor pool
	 */
	void destroy() noexcept {
		this->~VulkanDescriptorPool();
	}

	VulkanDescriptorPool operator=(const VulkanDescriptorPool&) const noexcept = delete;

	/**
	 * @brief create a descriptor pool to allocate the descriptor sets
	 *
	 * @param swapchain swapchain
	 */
	void create(const Builder builder);

	/**
	 * @brief get the descriptor pool
	 *
	 * @return const VkDescriptorPool&
	 */
	[[nodiscard]] const VkDescriptorPool& get() const noexcept { return _descriptorPool; }

private:
	VkDescriptorPool _descriptorPool = VK_NULL_HANDLE;
};

/**
 * @brief wrapper around the Vulkan descriptor set
 */
class VulkanDescriptor {
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
		 * @brief destructor of the wrter
		 */
		~Writer() noexcept {
			writes.clear();
		}

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

	VulkanDescriptor();

	VulkanDescriptor operator=(const VulkanDescriptor&) const noexcept = delete;

	/**
	 * @brief construct new Vulkan descriptors
	 *
	 * @param layout descriptor set layout
	 * @param pool descriptor pool
	 * @param writer data to be written into the descriptor
	 */
	void create(const VulkanDescriptorSetLayout* const layout, const VulkanDescriptorPool* const pool, Writer writer);

	/**
	 * @brief get the descriptor set
	 *
	 * @return const VkDescriptorSet&
	 */
	[[nodiscard]] const VkDescriptorSet& get() const noexcept { return _descriptorSet; }

	/**
	 * @brief get the descriptor set
	 *
	 * @return const VkDescriptorSet* const
	 */
	[[nodiscard]] const VkDescriptorSet* const get_ptr() const noexcept { return &_descriptorSet; }

private:
	VkDescriptorSet _descriptorSet = VK_NULL_HANDLE;
};

} // namespace lyra
