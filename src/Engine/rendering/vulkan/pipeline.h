/*************************
 * @file pipeline.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief wrappers around the Vulkan graphics pipeline
 * 
 * @date 2022-02-05
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <core/defines.h>
#include <res/loaders/load_file.h>
#include <rendering/vulkan/swapchain.h>
#include <rendering/vulkan/devices.h>
#include <rendering/vulkan/descriptor.h>
#include <rendering/vulkan/framebuffer.h>
#include <res/mesh.h>
#include <res/vulkan_shader.h>
#include <core/logger.h>

#include <vector>

#include <vulkan/vulkan.h>

namespace lyra {
/**
 * @brief wrapper around the Vulkan graphics pipeline
 * 
 */
class VulkanGraphicsPipeline {
private:
	/**
	 * @brief creation information of a pipeline
	 */
	struct 							VulkanGraphicsPipelineCreateInfo {
		std::vector <VkPipelineShaderStageCreateInfo> 	shaderStages;
		VkPipelineVertexInputStateCreateInfo 			vertexInputInfo;
		VkPipelineInputAssemblyStateCreateInfo 			inputAssembly;
		VkPipelineTessellationStateCreateInfo			tesselation;
		VkViewport 										viewport;
		VkRect2D 										scissor;
		VkPipelineViewportStateCreateInfo 				viewportState;
		VkPipelineRasterizationStateCreateInfo 			rasterizer;
		VkPipelineMultisampleStateCreateInfo 			multisampling;
		VkPipelineDepthStencilStateCreateInfo 			depthStencilState;
		VkPipelineColorBlendAttachmentState 			colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo 			colorBlending;
		std::vector <VkDynamicState>					dynamicStates;
		VkPipelineDynamicStateCreateInfo 				dynamicState;
	};

	/**
	 * @brief struct containling all the variabels
	 */
	struct 				Variables {
		VkPipeline 			graphicsPipeline;
		VkPipelineLayout 	pipelineLayout;
		
		VulkanShader 		vertShader;
		VulkanShader 		fragShader;
	};

public:
	VulkanGraphicsPipeline();

	/**
	 * @brief destroy the VulkanGraphicsPipeline 
	 * @brief works like the destructor, but can be called anywhere
	 * 
	 * @param device device
	 */
	void 				destroy();

	/**
	 * @brief create a new graphics pipeline
	 * 
	 * @param device device
	 * @param swapchain swapchain
	 * @param framebuffer framebuffer
	 * @param descriptor descriptor sets
	 * @param commandBuffer command buffer
	 * @param vertPath path of the vertex shader
	 * @param fragPath path of the fragment shader
	 */
	void 				create(VulkanDevice device, VulkanSwapchain swapchain, const VulkanFramebuffers framebuffer, 
		const VulkanDescriptorSetLayout descriptorSetLayout, const std::string vertPath, const std::string fragPath);
	
	/**
	 * @brief get all the variabels
	 * 
	 * @return Variables 
	 */
	Variables 			get() const;

private:
	Variables 			var;

	VulkanDevice*		device;
	VulkanSwapchain*	swapchain;


	/**
	 * @brief create the graphics pipeline
	 * 
	 * @param vertPath path of the vertex shader
	 * @param fragPath path of the fragment shader
	 * @param descriptor descriptors
	 */
	void 				create_pipeline(const std::string vertPath, const std::string fragPath, const VulkanFramebuffers framebuffer, 
		const VulkanDescriptorSetLayout descriptorSetLayout);
	/**
	 * @brief create the pipeline layout
	 * 
	 * @param descriptor descriptors
	 */
	void 				create_layout(const VulkanDescriptorSetLayout descriptorSetLayout);

};

} // namespace lyra
