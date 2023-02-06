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

#include <lyra.h>

#include <vector>

#include <vulkan/vulkan.h>

#include <core/smart_pointer.h>

#include <rendering/vulkan/vulkan_shader.h>
#include <rendering/vulkan/descriptor.h>

namespace lyra {

namespace vulkan {

/**
 * @brief wrapper around the Vulkan graphics pipeline
 */
class Pipeline {
public:
	/**
	 * @brief descriptor and shader information
	 */
	struct Binding {
		// shader type
		const int& shaderType;
		// the descriptor set layout this binding belongs to
		const uint32& descriptorSetLayoutIndex;
		// type of descriptor
		const int& descriptorType;
		// multiplier for the number of descriptors to allocate
		const uint32& descriptorAllocCountMultiplier;
		// number of descriptors in that slot (array)
		const uint32& arraySize = 1;
	};

	/**
	 * @brief struct holding creation information for a shader
	 */
	struct ShaderInfo {
		// shader type
		const int& type;
		// path of shader
		const char* path;
		// shader entry point
		const char* entry = "main";
	};

	/**
	 * @brief destructor of the pipeline
	 */
	virtual ~Pipeline() noexcept;

	Pipeline operator=(const Pipeline&) const noexcept = delete;

	/**
	 * @brief get the descriptor management system
	 *
	 * @return constexpr const DescriptorSystem&
	*/
	NODISCARD constexpr const DescriptorSystem& descriptorSystem() const noexcept { return m_descriptorSystem; }
	/**
	 * @brief get the pipeline
	 *
	 * @return constexpr VkPipeline
	*/
	NODISCARD constexpr VkPipeline pipeline() const noexcept { return m_pipeline; }
	/**
	 * @brief get the pipeline layout
	 *
	 * @return constexpr VkPipelineLayout
	*/
	NODISCARD constexpr VkPipelineLayout layout() const noexcept { return m_layout; }
	/**
	 * @brief get the shaders
	 *
	 * @return const std::vector<Shader>
	*/
	NODISCARD const std::vector<Shader> shaders() const noexcept { return m_shaders; }
	/**
	 * @brief get the shaders
	 *
	 * @return constexpr VkPipelineBindPoint
	*/
	NODISCARD constexpr VkPipelineBindPoint bindPoint() const noexcept { return m_bindPoint; }

protected:
	VkPipeline m_pipeline;
	VkPipelineLayout m_layout;
	DescriptorSystem m_descriptorSystem;

	VkPipelineBindPoint m_bindPoint;

	std::vector<Shader> m_shaders;

	/**
	 * @brief create the pipeline layout
	 * 
	 * @param pushConstants push constant data
	 */
	void create_layout(const std::vector<VkPushConstantRange>& pushConstants); // first time I've used a default Vulkan struct in an exposed part of the API
	/**
	 * @brief create stuff related to descriptors
	 * 
	 * @param bindings descriptorinformation. Have to be in their correct order
	 * @param poolFlags descriptor pool additional flags
	 * @param maxSets maximum number of descriptor sets
	*/
	void create_descriptor_stuff(const std::vector<Binding>& bindings, const VkDescriptorPoolCreateFlags& poolFlags = 0);
	/**
	 * @brief create all the shaders
	 *
	 * @param shaderInfo shader information
	 */
	void create_shaders(const std::vector<ShaderInfo>& shaderInfo);
};

} // namespace vulkan

} // namespace lyra
