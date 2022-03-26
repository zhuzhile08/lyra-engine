/*************************
 * @file pipeline.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief wrapper around the Vulkan graphics pipeline
 * @brief loads shaders and processes the vertices passing through
 *
 * @date 2022-02-05
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <core/defines.h>
#include <res/loaders/load_file.h>
#include <rendering/vulkan/swapchain.h>
#include <rendering/vulkan/devices.h>
#include <rendering/vulkan/descriptor.h>
#include <rendering/vulkan/framebuffer.h>
#include <res/vertex.h>
#include <rendering/vulkan/vulkan_shader.h>
#include <core/logger.h>

#include <vector>

#include <vulkan/vulkan.h>

namespace lyra {
/**
 * @brief wrapper around the Vulkan graphics pipeline
 */
class VulkanGraphicsPipeline {
private:
	/**
	 * @brief creation information of a pipeline
	 */
	struct VulkanGraphicsPipelineCreateInfo {
		std::vector <VkPipelineShaderStageCreateInfo> shaderStages;
		VkPipelineVertexInputStateCreateInfo vertexInputInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssembly;
		VkPipelineTessellationStateCreateInfo tesselation;
		VkViewport viewport;
		VkRect2D scissor;
		VkPipelineViewportStateCreateInfo viewportState;
		VkPipelineRasterizationStateCreateInfo rasterizer;
		VkPipelineMultisampleStateCreateInfo multisampling;
		VkPipelineDepthStencilStateCreateInfo depthStencilState;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlending;
		std::vector <VkDynamicState> dynamicStates;
		VkPipelineDynamicStateCreateInfo dynamicState;
	};

	/**
	 * @brief struct holding creation information for a shader
	 */
	struct ShaderCreationInfo {
		str path;
		str entry;
		VkShaderStageFlagBits flag;
	};

public:
	VulkanGraphicsPipeline();

	/**
	 * @brief destroy the VulkanGraphicsPipeline
	 * @brief works like the destructor, but can be called anywhere
	 *
	 * @param device device
	 */
	void destroy() noexcept;

	/**
	 * @brief create a new graphics pipeline
	 *
	 * @param device device
	 * @param framebuffer framebuffer
	 * @param descriptorSetLayout descriptor set layout
	 * @param shaderCount number of shaders
	 * @param shaderCreationInfos creation information of the shaders
	 * @param size size of which part the pipeline will draw to
	 * @param area area of where the pipeline can draw to
	 */
	void create(
		VulkanDevice                            device,
		const VulkanFramebuffers                framebuffer,
		const VulkanDescriptorSetLayout         descriptorSetLayout,
		const std::vector<ShaderCreationInfo>   shaderCreationInfos,
		VkExtent2D                              size,
		VkExtent2D                              area
	);

	/**
	 * @brief get the graphics pipeline
	 * 
	 * @return const VkPipeline
	*/
	const VkPipeline graphicsPipeline() const noexcept { return _graphicsPipeline; }
	const VkPipelineLayout pipelineLayout() const noexcept { return _pipelineLayout; }
	const std::vector<VulkanShader> shaders() const noexcept { return _shaders; }

private:
	VkPipeline _graphicsPipeline;
	VkPipelineLayout _pipelineLayout;

	std::vector<VulkanShader> _shaders;

	VulkanDevice* device;

	/**
	 * @brief create the graphics pipeline
	 *
	 * @param framebuffer framebuffer
	 * @param descriptor descriptors
	 * @param size size of which part the pipeline will draw to
	 * @param area area of where the pipeline can draw to
	 */
	void create_pipeline(const VulkanFramebuffers framebuffer, const VulkanDescriptorSetLayout descriptorSetLayout, VkExtent2D size, VkExtent2D area);
	/**
	 * @brief create the pipeline layout
	 *
	 * @param descriptor descriptors
	 */
	void create_layout(const VulkanDescriptorSetLayout descriptorSetLayout);
	/**
	 * @brief create all the shaders
	 * @brief minor caveat: since all the shaders are stored in a vector, you can only refer to them by index (as far as I know)
	 * @brief you have to memorise in which order you have created them
	 *
	 * @param shaderCreationInfos creation information of the shaders
	 */
	void create_shaders(std::vector<ShaderCreationInfo> shaderCreationInfos);
};

} // namespace lyra
