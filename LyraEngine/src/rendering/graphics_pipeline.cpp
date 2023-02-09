#include <rendering/graphics_pipeline.h>

#include <rendering/vulkan/vulkan_window.h>
#include <rendering/vulkan/devices.h>
#include <rendering/vulkan/descriptor.h>
#include <rendering/vulkan/vulkan_shader.h>
#include <nodes/mesh/mesh.h>
#include <nodes/graphics/camera.h>
#include <core/application.h>

namespace lyra {

GraphicsPipeline::Builder::Builder(const Renderer* const renderer) : m_renderer(renderer) {
	// temporary for mesh binding description
	const auto meshBindingDescription = Mesh::Vertex::get_binding_description();

	// setup the basic create info
	m_createInfo = GraphicsPipelineCreateInfo {
		// shader information
		{ },
		{	// describe how vertices are inputed into shaders
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			nullptr,
			0,
			1,
			&meshBindingDescription,
			static_cast<uint32>(Mesh::Vertex::get_attribute_descriptions().size()),
			Mesh::Vertex::get_attribute_descriptions().data()
		},
		{	// describe how shaders are executed
			VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			VK_FALSE
		},
		{	// tesselation

		},
		{	// define where the drawable area on the window is
			0.0f,
			0.0f,
			static_cast<float>(Application::renderSystem.vulkanWindow.extent().width),
			static_cast<float>(Application::renderSystem.vulkanWindow.extent().height),
			0.0f,
			1.0f
		},
		{	// define where you will acutally draw to
			{0, 0},
			Application::renderSystem.vulkanWindow.extent()
		},
		{
			VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			nullptr,
			0,
			1,
			&m_createInfo.viewport,
			1,
			&m_createInfo.scissor
		},
		{	// create the rasteriser to create the fragments
			VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_FALSE,
			VK_FALSE,
			VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_BACK_BIT,
			VK_FRONT_FACE_COUNTER_CLOCKWISE,
			VK_FALSE,
			0.0f,
			0.0f,
			0.0f,
			1.0f
		},
		{	// configure anti alising
			VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			nullptr,
			0,
			Application::renderSystem.vulkanWindow.maxMultisamples(),
			VK_FALSE,
			0.0f
		},
		{	// depth buffering
			VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_TRUE,
			VK_TRUE,
			VK_COMPARE_OP_LESS,
			VK_FALSE,
			VK_FALSE
		},
		{	// configure color blending
			VK_FALSE,
			VK_BLEND_FACTOR_SRC_ALPHA,
			VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			VK_BLEND_OP_ADD,
			VK_BLEND_FACTOR_ONE,
			VK_BLEND_FACTOR_ZERO,
			VK_BLEND_OP_ADD,
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
		},
		{
			VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_FALSE,
			VK_LOGIC_OP_COPY,
			1,
			&m_createInfo.colorBlendAttachment,
			{
				0.0f, 0.0f, 0.0f, 0.0f
			}
		},
		// dynamic state config
		{ },
		{
			VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			nullptr,
			0,
			0,
			nullptr
		} 
	};
}

void GraphicsPipeline::Builder::create_shader_stages(const GraphicsPipeline& graphicsPipeline) {
	// add all the shader stage creation information into a vector
	std::vector <VkPipelineShaderStageCreateInfo> shaderStages;
	shaderStages.reserve(graphicsPipeline.shaders().size());
	for (const auto& shader : graphicsPipeline.shaders()) shaderStages.push_back(shader.get_stage_create_info());

	// set the shader stages in the create info
	m_createInfo.shaderStages = shaderStages;
}

GraphicsPipeline::GraphicsPipeline(Builder& builder) {
	
	// define what type of pipeline this is
	m_bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

	// crate shaders
	create_shaders(&builder);
	builder.create_shader_stages(*this);
	// create stuff relating to descriptors
	create_descriptor_stuff(&builder); // yes, I know, very good naming
	// create the pipeline layout
	create_layout(&builder);

	const auto createInfo = builder.build_pipeline_create_info(*this);
	vassert(vkCreateGraphicsPipelines(Application::renderSystem.device.device(), VK_NULL_HANDLE, 1, &createInfo, nullptr, &m_pipeline), "create Vulkan pipeline");
}

} // namespace lyra
