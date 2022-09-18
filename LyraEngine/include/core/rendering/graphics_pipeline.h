/*************************
 * @file graphics_pipeline.h
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

#include <core/rendering/vulkan/vulkan_pipeline.h>
#include <core/rendering/renderer.h>
#include <core/decl.h>

#include <vector>

#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief wrapper around the Vulkan graphics pipeline
 */
class GraphicsPipeline : public vulkan::Pipeline {
private:
	/**
	 * @brief creation information of a pipeline
	 */
	struct GraphicsPipelineCreateInfo {
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

public:
	// color blending mode
	enum class ColorBlending {
		// enable color blending
		BLEND_ENABLE = 1U,
		// disable color blending
		BLEND_DISABLE = 0U
	};

	// color blending for the brits out there
	typedef ColorBlending Colourblending;

	// tessellation mode
	enum class Tessellation {
		// enable tessellation
		TESSELLATION_ENABLE = 1U,
		// disable tessellation
		TESSELLATION_DISABLE = 0U
	};

	// multismpling mode
	enum class Multisampling {
		// enable multisampling
		MULTISAMPLING_ENABLE = 1U,
		// disable multisampling
		MULTISAMPLING_DISABLE = 0U
	};

	// polygon rendering mode
	enum class RenderMode : int {
		// fill polygons
		MODE_FILL = 0,
		// draw lines
		MODE_LINE = 1,
		// draw points
		MODE_POINT = 2
	};

	// culling mode 
	enum class Culling {
		// no culling
		CULLING_NONE = 0x00000000,
		// cull front face
		CULLING_FRONT = 0x00000001,
		// cull back face
		CULLING_BACK = 0x00000002,
		// cull all faces
		CULLING_ALL = 0x00000003
	};

	GraphicsPipeline() { }

	/**
	 * @brief construct a new graphics pipeline
	 * 
	 * @param renderer renderer to render the pipeline
	 * @param shaders shader creation information
	 * @param bindings descriptor binding information
	 * @param pushConstants push constant information
	 * @param size draw size
	 * @param area draw area
	 * @param colorBlending enable color blending
	 * @param tessellation enable tessellation
	 * @param multisampling enable multisampling
	 * @param renderMode polygon rendering mode
	 * @param culling culling mode
	 */
	GraphicsPipeline(
		const Renderer* const renderer,
		const std::vector<ShaderInfo> shaders,
		const std::vector<Binding> bindings,
		const std::vector<VkPushConstantRange> pushConstants,
		const VkExtent2D size,
		const VkExtent2D area,
		const ColorBlending&& colorBlending = ColorBlending::BLEND_ENABLE,
		const Tessellation&& tessellation = Tessellation::TESSELLATION_ENABLE,
		const Multisampling&& multisampling = Multisampling::MULTISAMPLING_ENABLE,
		const RenderMode&& renderMode = RenderMode::MODE_FILL,
		const Culling&& culling = Culling::CULLING_BACK
	);

private:
	/**
	 * @brief create a pipeline
	 * 
	 * @param renderer renderer to render the pipeline
	 * @param pushConstants push constant information
	 * @param size draw size
	 * @param area draw area
	 * @param colorBlending enable color blending
	 * @param tessellation enable tessellation
	 * @param multisampling enable multisampling
	 * @param renderMode polygon rendering mode
	 * @param culling culling mode
	 */
	void create_pipeline(
		const Renderer* const renderer,
		const std::vector<VkPushConstantRange> pushConstants,
		const VkExtent2D size,
		const VkExtent2D area,
		const ColorBlending colorBlending,
		const Tessellation tessellation,
		const Multisampling multisampling,
		const RenderMode renderMode,
		const Culling culling
	);
};

} // namespace lyra
