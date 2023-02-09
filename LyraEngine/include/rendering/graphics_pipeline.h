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

		/**
		 * @brief create the shader stages of the create info
		 */
		void create_shader_stages(const GraphicsPipeline& graphicsPipeline);

		/**
		 * @brief build the actual pipeline creation information
		 * 
		 * @param graphicsPipeline pipeline the pipeline layout belongs to
		 * 
		 * @return VkGraphicsPipelineCreateInfo 
		 */
		VkGraphicsPipelineCreateInfo build_pipeline_create_info(const GraphicsPipeline& graphicsPipeline) const noexcept {
			return {
				VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,	
				nullptr,
				0,
				static_cast<uint32>(m_createInfo.shaderStages.size()),
				m_createInfo.shaderStages.data(),
				&m_createInfo.vertexInputInfo,
				&m_createInfo.inputAssembly,
				&m_createInfo.tesselation,
				&m_createInfo.viewportState,
				&m_createInfo.rasterizer,
				&m_createInfo.multisampling,
				&m_createInfo.depthStencilState,
				&m_createInfo.colorBlending,
				&m_createInfo.dynamicState,
				graphicsPipeline.layout(),
				m_renderer->renderPass(),
				0,
				VK_NULL_HANDLE,
				0
			};
		}

		friend class GraphicsPipeline;
	};

	/**
	 * @brief construct a new graphics pipeline
	 * 
	 * @param builder builder that contains the information to build the pipeline
	 */
	GraphicsPipeline(Builder& builder);
};

} // namespace lyra
