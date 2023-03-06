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

#include <lyra.h>

#include <rendering/vulkan/vulkan_pipeline.h>
#include <rendering/renderer.h>

#include <vector>

#include <vulkan/vulkan.h>
#include <rendering/vulkan/vulkan_raii.h>

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
		VkVertexInputBindingDescription meshBindingDescription;
		Array<VkVertexInputAttributeDescription, 4> meshAttributeDescriptions;
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
	class Builder : public vulkan::Pipeline::Builder {
	public:
		// color blending mode
		enum class ColorBlending {
			// enable color blending
			BLEND_ENABLE = 1U,
			// disable color blending
			BLEND_DISABLE = 0U
		};

		// color blending for the brits out there
		using Colourblending = ColorBlending;

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

		// polygon front face
		enum class PolygonFrontFace : int {
			// read the polygon counter clockwise
			FRONT_FACE_COUNTER_CLOCKWISE = 0,
			// read the polzgon clockwise
			FRONT_FACE_CLOCKWISE = 1
		};

		/**
		 * @brief Construct a new Builder object
		 * 
		 * @param graphicsPipeline the graphicsPipeline that it will create
		 * @param renderer renderer the renderpass belongs to
		 */
		Builder(const Renderer* const renderer);

		/**
		 * @brief enable and configure sample shading
		 * 
		 * @param strength strength of the sample shading, from 0 to 1
		 */
		void enable_sample_shading(float strength) {
			m_createInfo.multisampling.sampleShadingEnable = VK_TRUE;
			m_createInfo.multisampling.minSampleShading = strength;
		}
		/**
		 * @brief set the culling mode
		 * 
		 * @param cullingMode culling mode
		 */
		void set_culling_mode(Culling cullingMode) {
			m_createInfo.rasterizer.cullMode = static_cast<VkCullModeFlags>(cullingMode);
		}
		/**
		 * @brief set the rendering mode
		 * 
		 * @param renderMode rendering mode
		 */
		void set_render_mode(RenderMode renderMode) {
			m_createInfo.rasterizer.polygonMode = static_cast<VkPolygonMode>(renderMode);
		}
		/**
		 * @brief set the polyon front face
		 * 
		 * @param frontFace polygon front face
		 */
		void set_polyon_front_face(PolygonFrontFace frontFace) {
			m_createInfo.rasterizer.frontFace = static_cast<VkFrontFace>(frontFace);
		}

		/// @todo make color blending and dynamic state possible
		/// @todo also make the scissor and viewport customizable

	private:
		GraphicsPipelineCreateInfo m_createInfo;
		const Renderer* const m_renderer;

		void build_graphics_pipeline(GraphicsPipeline* const graphicsPipeline) const;

		friend class GraphicsPipeline;
	};

	GraphicsPipeline() = default;
	/**
	 * @brief construct a new graphics pipeline
	 * 
	 * @param builder builder that contains the information to build the pipeline
	 */
	GraphicsPipeline(const Builder& builder);
	DEFINE_DEFAULT_MOVE(GraphicsPipeline)

	friend class Builder;
};

} // namespace lyra
