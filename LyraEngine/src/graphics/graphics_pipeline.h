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

#include <core/defines.h>
#include <core/rendering/vulkan/vulkan_window.h>
#include <core/rendering/vulkan/devices.h>
#include <core/rendering/vulkan/vulkan_pipeline.h>
#include <core/rendering/vulkan/descriptor.h>
#include <core/rendering/vulkan/vulkan_shader.h>
#include <components/mesh/mesh.h>
#include <components/graphics/camera.h>
#include <core/logger.h>
#include <lyra.h>

#include <vector>

#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief wrapper around the Vulkan graphics pipeline
 */
class GraphicsPipeline : public VulkanPipeline {
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

	// creation information
	struct CreateInfo {
		// camera to render the pipeline
		Camera* const camera;
		// shader creation information
		const std::vector<ShaderCreationInfo> shaderCreationInfos;
		// descriptor set layout and pool creation information
		const Builder builder;
		// draw size
		const VkExtent2D size;
		// draw area
		const VkExtent2D area;
		// enable color blending
		const ColorBlending colorBlending;
		// enable tessellation
		const Tessellation tessellation;
		// enable multisampling
		const Multisampling multisampling;
		// polygon rendering mode
		const RenderMode renderMode = RenderMode::MODE_FILL;
		// culling mode
		const Culling culling = Culling::CULLING_BACK;
	};

	GraphicsPipeline() { }

	/**
	 * @brief create a new graphics pipeline
	 *
	 * @param info creation information
	 */
	void create(const CreateInfo info);

private:
	/**
	 * @brief create the graphics pipeline
	 *
	 * @param info creation information
	 */
	void create_pipeline(const CreateInfo& info);
};

} // namespace lyra
