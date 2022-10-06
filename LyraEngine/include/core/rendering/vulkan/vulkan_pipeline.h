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

#include <vector>

#include <vulkan/vulkan.h>

#include <core/decl.h>

namespace lyra {

namespace vulkan {

/**
 * @brief wrapper around the Vulkan graphics pipeline
 */
class Pipeline {
public:
	Pipeline() { }

	/**
	 * @brief descriptor and shader information
	 */
	struct Binding {
		// type of descriptor
		int descriptorType;
		// number of this type of descriptor needed in the shader
		uint32 descriptorCount;
		// number of descriptors to allocate
		uint32 descriptorAllocCount;
		// shader type
		const int shaderType;
	};

	/**
	 * @brief struct holding creation information for a shader
	 */
	struct ShaderInfo {
		// shader type
		const int type;
		// path of shader
		const char* path;
		// shader entry point
		const char* entry = "main";
	};

	/**
	 * @brief destructor of the pipeline
	 */
	virtual ~Pipeline() noexcept;
	/**
	 * @brief destroy the pipeline
	 */
	virtual void destroy() noexcept { this->~Pipeline(); };

	Pipeline operator=(const Pipeline&) const noexcept = delete;

	/**
	 * @brief get the descriptor set layout
	 *
	 * @return const DescriptorSetLayout* const
	*/
	NODISCARD const DescriptorSetLayout* const descriptorSetLayout() const noexcept { return m_descriptorSetLayout; }
	/**
	 * @brief get the descriptor pool
	 *
	 * @return const DescriptorPool* const
	*/
	NODISCARD const DescriptorPool* const descriptorPool() const noexcept { return m_descriptorPool; }
	/**
	 * @brief get the pipeline
	 *
	 * @return const VkPipeline&
	*/
	NODISCARD const VkPipeline& pipeline() const noexcept { return m_pipeline; }
	/**
	 * @brief get the pipeline layout
	 *
	 * @return const VkPipelineLayout&
	*/
	NODISCARD const VkPipelineLayout& layout() const noexcept { return m_layout; }
	/**
	 * @brief get the shaders
	 *
	 * @return const std::vector<Shader>&
	*/
	NODISCARD const std::vector<Shader>& shaders() const noexcept { return m_shaders; }
	/**
	 * @brief get the shaders
	 *
	 * @return const VkPipelineBindPoint&
	*/
	NODISCARD const VkPipelineBindPoint& bindPoint() const noexcept { return m_bindPoint; }

protected:
	VkPipeline m_pipeline = VK_NULL_HANDLE;
	VkPipelineLayout m_layout = VK_NULL_HANDLE;
	SmartPointer<DescriptorSetLayout> m_descriptorSetLayout;
	SmartPointer<DescriptorPool> m_descriptorPool;

	VkPipelineBindPoint m_bindPoint;

	std::vector<Shader> m_shaders;

	/**
	 * @brief create the pipeline layout
	 * 
	 * @param pushConstants push constant data
	 */
	void create_layout(std::vector<VkPushConstantRange> pushConstants); // first time I've used a default Vulkan struct in an exposed part of the API
	/**
	 * @brief create stuff related to descriptors
	 * 
	 * @param bindings descriptorinformation. Have to be in their correct order
	 * @param poolFlags descriptor pool additional flags
	 * @param maxSets maximum number of descriptor sets
	*/
	void create_descriptor_stuff(std::vector<Binding> bindings, VkDescriptorPoolCreateFlags poolFlags = 0);
	/**
	 * @brief create all the shaders
	 *
	 * @param shaders shader information
	 */
	void create_shaders(std::vector<ShaderInfo> shaders);
};

} // namespace vulkan

} // namespace lyra
