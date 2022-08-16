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

#include <core/decl.h>
#include <core/application.h>

#include <vector>
#include <memory>

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
		const int type;
		// path of shader
		const char* path;
		// shader entry point
		const char* entry = "main";
	};

public:
	VulkanPipeline() { }

	// descriptor and shader information
	struct Binding {
		// type of descriptor
		int descriptorType;
		// number of this type of descriptor needed in the shader
		uint32 descriptorCount;
		// number of descriptor to allocate
		uint32 descriptorAllocCount;
		// type of shader
		int shaderType;
		// shader path
		const char* path;
		// shader entry function name
		const char* entry;
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
	 * @return const VulkanDescriptorSetLayout* const
	*/
	NODISCARD const VulkanDescriptorSetLayout* const descriptorSetLayout() const noexcept { return _descriptorSetLayout; }
	/**
	 * @brief get the descriptor pool
	 *
	 * @return const VulkanDescriptorPool* const
	*/
	NODISCARD const VulkanDescriptorPool* const descriptorPool() const noexcept { return _descriptorPool; }
	/**
	 * @brief get the pipeline
	 *
	 * @return const VkPipeline&
	*/
	NODISCARD const VkPipeline& pipeline() const noexcept { return _pipeline; }
	/**
	 * @brief get the pipeline layout
	 *
	 * @return const VkPipelineLayout&
	*/
	NODISCARD const VkPipelineLayout& layout() const noexcept { return _layout; }
	/**
	 * @brief get the shaders
	 *
	 * @return const std::vector<VulkanShader>&
	*/
	NODISCARD const std::vector<VulkanShader>& shaders() const noexcept { return _shaders; }
	/**
	 * @brief get the shaders
	 *
	 * @return const VkPipelineBindPoint&
	*/
	NODISCARD const VkPipelineBindPoint& bindPoint() const noexcept { return _bindPoint; }

protected:
	VkPipeline _pipeline = VK_NULL_HANDLE;
	VkPipelineLayout _layout = VK_NULL_HANDLE;
	SmartPointer<VulkanDescriptorSetLayout> _descriptorSetLayout;
	SmartPointer<VulkanDescriptorPool> _descriptorPool;

	VkPipelineBindPoint _bindPoint;

	std::vector<VulkanShader> _shaders;

	/**
	 * @brief create the pipeline layout
	 */
	void create_layout();

	/**
	 * @brief create stuff related to descriptors
	 * 
	 * @param bindings descriptor and shader information. Have to be in their correct order
	 * @param poolFlags descriptor pool additional flags
	 * @param maxSets maximum number of descriptor sets
	*/
	void create_descriptor_stuff(std::vector<Binding> bindings, VkDescriptorPoolCreateFlags poolFlags = 0);

private:
	/**
	 * @brief create all the shaders
	 *
	 * @param bindings descriptor and shader information. Have to be in their correct order
	 */
	void create_shaders(std::vector<Binding> bindings);
};

} // namespace lyra
