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
#include <core/rendering/vulkan/swapchain.h>
#include <core/rendering/vulkan/devices.h>
#include <core/rendering/vulkan/descriptor.h>
#include <core/rendering/vulkan/framebuffers.h>
#include <core/rendering/vulkan/vulkan_shader.h>
#include <core/rendering/vulkan/vertex.h>
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
		std::string path;
		std::string entry;
		VkShaderStageFlagBits flag;
	};

public:
	VulkanGraphicsPipeline();

	/**
	 * @brief destructor of the pipeline
	 */
	virtual ~VulkanGraphicsPipeline() noexcept;

	/**
	 * @brief destroy the pipeline
	 */
	void destroy() noexcept;

	VulkanGraphicsPipeline operator=(const VulkanGraphicsPipeline&) const noexcept = delete;

	/**
	 * @brief create a new graphics pipeline
	 *
	 * @param device device
	 * @param framebuffer framebuffer
	 * @param descriptorSetLayout descriptor set layout
	 * @param shaderCreationInfos creation information of the shaders
	 * @param size size of which part the pipeline will draw to
	 * @param area area of where the pipeline can draw to
	 * @param polyMode how the polygons will be filled
	 */
	void create(
		const VulkanDevice* device,
		const VulkanFramebuffers* framebuffer,
		const VulkanDescriptorSetLayout* descriptorSetLayout,
		const std::vector<ShaderCreationInfo> shaderCreationInfos,
		VkExtent2D size,
		VkExtent2D area, 
		VkPolygonMode polyMode = VK_POLYGON_MODE_FILL
	);

	/**
	 * @brief get the graphics pipeline
	 * 
	 * @return const VkPipeline
	*/
	[[nodiscard]] const VkPipeline graphicsPipeline() const noexcept { return _graphicsPipeline; }
	/**
	 * @brief get the pipeline layout
	 * 
	 * @return const VkPipelineLayout
	*/
	[[nodiscard]] const VkPipelineLayout pipelineLayout() const noexcept { return _pipelineLayout; }
	/**
	 * @brief get the shaders
	 * 
	 * @return const std::vector<VulkanShader>
	*/
	[[nodiscard]] const std::vector<VulkanShader> shaders() const noexcept { return _shaders; }

private:
	VkPipeline _graphicsPipeline = VK_NULL_HANDLE;
	VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;

	std::vector<VulkanShader> _shaders;

	const VulkanDevice* device;

	/**
	 * @brief create the graphics pipeline
	 *
	 * @param framebuffer framebuffer
	 * @param descriptor descriptors
	 * @param size size of which part the pipeline will draw to
	 * @param area area of where the pipeline can draw to
	 * @param polyMode how the polygons will be filled
	 */
	void create_pipeline(const VulkanFramebuffers* framebuffer, const VulkanDescriptorSetLayout* descriptorSetLayout, VkExtent2D size, VkExtent2D area, VkPolygonMode polyMode);
	/**
	 * @brief create the pipeline layout
	 *
	 * @param descriptor descriptors
	 */
	void create_layout(const VulkanDescriptorSetLayout* descriptorSetLayout);
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