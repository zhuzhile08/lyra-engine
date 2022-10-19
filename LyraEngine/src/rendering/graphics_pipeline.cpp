#include <rendering/graphics_pipeline.h>

#include <rendering/vulkan/vulkan_window.h>
#include <rendering/vulkan/devices.h>
#include <rendering/vulkan/descriptor.h>
#include <rendering/vulkan/vulkan_shader.h>
#include <nodes/mesh/mesh.h>
#include <nodes/graphics/camera.h>
#include <core/application.h>


namespace lyra {

GraphicsPipeline::GraphicsPipeline(
	const Renderer* const renderer,
	const std::vector<ShaderInfo> shaders,
	const std::vector<Binding> bindings,
	const std::vector<VkPushConstantRange> pushConstants,
	const VkExtent2D size,
	const VkExtent2D area,
	const ColorBlending&& colorBlending,
	const Tessellation&& tessellation,
	const Multisampling&& multisampling,
	const RenderMode&& renderMode,
	const Culling&& culling
) {
	Logger::log_info("Creating Vulkan graphics pipeline...");

	// define what type of pipeline this is
	m_bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

	// crate shaders
	create_shaders(shaders);
	// create stuff relating to descriptors
	create_descriptor_stuff(std::move(bindings)); // yes, I know, very good naming

	// create the pipeline
	create_pipeline(
		std::move(renderer),
		std::move(pushConstants),
		std::move(size),
		std::move(area), 
		std::move(colorBlending), 
		std::move(tessellation), 
		std::move(multisampling), 
		std::move(renderMode), 
		std::move(culling)
	);

	Logger::log_info("Successfully created Vulkan pipeline at ", get_address(this), "!", Logger::end_l());
}

void GraphicsPipeline::create_pipeline(
	const Renderer* const renderer,
	const std::vector<VkPushConstantRange> pushConstants,
	const VkExtent2D size,
	const VkExtent2D area,
	const ColorBlending colorBlending,
	const Tessellation tessellation,
	const Multisampling multisampling,
	const RenderMode renderMode,
	const Culling culling
) {
	// add all the shader stage creation information into a vector
	std::vector <VkPipelineShaderStageCreateInfo> shaderStages;
	shaderStages.resize(m_shaders.size());
	for (uint32 i = 0; i < shaderStages.size(); i++) shaderStages.at(i) = m_shaders.at(i).get_stage_create_info();

	auto temp = Mesh::Vertex::get_binding_description(); // i loooove c++
	auto temp2 = Mesh::Vertex::get_attribute_descriptions();

	GraphicsPipelineCreateInfo createInfo = {
		shaderStages, // create shaders
		{	// describe how vertices are inputed into shaders
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			nullptr,
			0,
			1,
			&temp,
			static_cast<uint32>(temp2.size()),
			temp2.data()
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
			(float) size.width,
			(float) size.height,
			0.0f,
			1.0f
		},
		{	// define where you will acutally draw to
			{0, 0},
			area
		},
		{
			VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			nullptr,
			0,
			1,
			&createInfo.viewport,
			1,
			&createInfo.scissor
		},
		{	// create the rasteriser to create the fragments
			VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_FALSE,
			VK_FALSE,
			static_cast<VkPolygonMode>(renderMode),
			static_cast<VkCullModeFlags>(culling),
			static_cast<VkFrontFace>(Settings::Rendering::polygonFrontFace),
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
			Application::renderSystem()->vulkanWindow()->maxMultisamples(),
			VK_TRUE,				// currently set to false
			0.2f,
			nullptr,
			VK_FALSE,
			VK_FALSE
		},
		{	// depth buffering
			VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_TRUE,
			VK_TRUE,
			VK_COMPARE_OP_LESS,
			VK_FALSE,
			VK_FALSE,
			{ },
			{ },
			0.0f,
			0.1f
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
			&createInfo.colorBlendAttachment,
			{
				0.0f, 0.0f, 0.0f, 0.0f
			}
		},
		{ 
			// VK_DYNAMIC_STATE_VIEWPORT,
			// VK_DYNAMIC_STATE_SCISSOR
		},
		{
			VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			nullptr,
			0,
			0,
			nullptr
		}
	};

	// create the pipeline layout
	create_layout(pushConstants);

	// create the pipeline
	VkGraphicsPipelineCreateInfo pipelineInfo = {
		VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,	
		nullptr,
		0,
		(uint32) createInfo.shaderStages.size(),
		createInfo.shaderStages.data(),
		&createInfo.vertexInputInfo,
		&createInfo.inputAssembly,
		&createInfo.tesselation,
		&createInfo.viewportState,
		&createInfo.rasterizer,
		&createInfo.multisampling,
		&createInfo.depthStencilState,
		&createInfo.colorBlending,
		&createInfo.dynamicState,
		m_layout,
		renderer->renderPass(),
		0,
		VK_NULL_HANDLE,
		0
	};

	vassert(vkCreateGraphicsPipelines(Application::renderSystem()->device()->device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline), "create Vulkan pipeline");
}

} // namespace lyra
