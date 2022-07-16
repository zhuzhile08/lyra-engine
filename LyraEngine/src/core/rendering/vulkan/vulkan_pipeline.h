/*************************
 * @file vulkan_pipeline.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief empty wrapper around the Vulkan pipeline
 *
 * @date 2022-02-05
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <core/defines.h>
#include <core/rendering/vulkan/vulkan_shader.h>
#include <core/rendering/vulkan/command_buffer.h>
#include <core/rendering/vulkan/descriptor.h>
#include <core/logger.h>
#include <lyra.h>

#include <vector>

#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief wrapper around the Vulkan graphics pipeline
 */
class VulkanPipeline {
protected:
	/**
	 * @brief struct holding creation information for a shader
	 */
	struct ShaderCreationInfo {
		// shader type
		const VulkanShader::Type type;
		// path of shader
		const char* path;
		// shader entry point
		const char* entry = "main";
	};

public:
	VulkanPipeline() { };

	/**
	 * @brief builder for the descriptor sets
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
		 * @brief add new bindings to the vector of bindings
		 *
		 * @param newBindings a vector with the data for a binding. Consists of the binding index, the type of descriptor to bind, the shader behind that descriptor, the number of descriptors needed for that binding and the actuall number of descriptors to allocate
		 */
		void add_bindings(std::vector<std::tuple<const uint32, const int, const VulkanShader::Type, const uint32, const uint32>> newBindings) noexcept {
			for (const auto& [binding, type, shaderType, count, allocCount] : newBindings) {
				bindings.push_back({
					binding,
					static_cast<VkDescriptorType>(type),
					count,
					static_cast<VkShaderStageFlags>(shaderType),
					nullptr
					});
				poolSizes.push_back({
					static_cast<VkDescriptorType>(type),
					allocCount
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
		 * @brief Set special flags for the pool
		 *
		 * @param _poolFlags
		 */
		void set_pool_flags(const VkDescriptorPoolCreateFlags _poolFlags) noexcept {
			poolFlags = _poolFlags;
		}

		std::vector<VkDescriptorPoolSize> poolSizes;
		std::vector<VkDescriptorSetLayoutBinding> bindings;
		VkDescriptorPoolCreateFlags poolFlags = 0;
		uint32 maxSets = 1000;
	};

	/**
	 * @brief destructor of the pipeline
	 */
	virtual ~VulkanPipeline() noexcept;
	/**
	 * @brief destroy the pipeline
	 */
	virtual void destroy() noexcept { this->~VulkanPipeline(); };

	VulkanPipeline operator=(const VulkanPipeline&) const noexcept = delete;

	/**
	 * @brief get the descriptor set layout
	 *
	 * @return const lyra::VulkanDescriptorSetLayout*
	*/
	[[nodiscard]] const VulkanDescriptorSetLayout* const descriptorSetLayout() const noexcept { return &_descriptorSetLayout; }
	/**
	 * @brief get the descriptor pool
	 *
	 * @return const lyra::VulkanDescriptorPool*
	*/
	[[nodiscard]] const VulkanDescriptorPool* const descriptorPool() const noexcept { return &_descriptorPool; }
	/**
	 * @brief get the pipeline
	 *
	 * @return const VkPipeline&
	*/
	[[nodiscard]] const VkPipeline& pipeline() const noexcept { return _pipeline; }
	/**
	 * @brief get the pipeline layout
	 *
	 * @return const VkPipelineLayout&
	*/
	[[nodiscard]] const VkPipelineLayout& layout() const noexcept { return _layout; }
	/**
	 * @brief get the shaders
	 *
	 * @return const std::vector<VulkanShader>&
	*/
	[[nodiscard]] const std::vector<VulkanShader>& shaders() const noexcept { return _shaders; }
	/**
	 * @brief get the shaders
	 *
	 * @return const VkPipelineBindPoint&
	*/
	[[nodiscard]] const VkPipelineBindPoint& bindPoint() const noexcept { return _bindPoint; }

protected:
	VkPipeline _pipeline = VK_NULL_HANDLE;
	VkPipelineLayout _layout = VK_NULL_HANDLE;
	VulkanDescriptorSetLayout _descriptorSetLayout;
	VulkanDescriptorPool _descriptorPool;

	VkPipelineBindPoint _bindPoint;

	std::vector<VulkanShader> _shaders;

	/**
	 * @brief create the pipeline layout
	 */
	void create_layout() {
		// create the pipeline layout
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{
			VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			nullptr,
			0,
			1,
			_descriptorSetLayout.get_ptr(),
			0,    /// @todo push constants
			nullptr
		};

		lassert(vkCreatePipelineLayout(Application::context()->device()->device(), &pipelineLayoutInfo, nullptr, &_layout) == VK_SUCCESS, "Failed to create Vulkan graphics pipeline layout!");
	}

	/**
	 * @brief create all the shaders
	 *
	 * @param shaderCreationInfos creation information of the shaders
	 */
	void create_shaders(std::vector<ShaderCreationInfo> shaderCreationInfos);

	/**
	 * @brief create stuff related to descriptors
	 * 
	 * @param builder pipeline builder
	*/
	void create_descriptor_stuff(Builder builder);
};

} // namespace lyra
