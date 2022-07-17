#include <graphics/graphics_pipeline.h>

namespace lyra {

void GraphicsPipeline::create(const CreateInfo info) {
	Logger::log_info("Creating Vulkan graphics pipeline...");

    _shaders.reserve(info.shaderCreationInfos.size());
	_shaders.resize(info.shaderCreationInfos.size());

	_bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

	create_shaders(info.shaderCreationInfos);

	// yes, I know, very good naming
	create_descriptor_stuff(info.builder);

	create_pipeline(info);

	Logger::log_info("Successfully created Vulkan pipeline at ", get_address(this), "!", Logger::end_l());
}

void GraphicsPipeline::create_pipeline(const CreateInfo& info) {
    // add all the shader stage creation information into a vector
    std::vector <VkPipelineShaderStageCreateInfo> shaderStages;
    shaderStages.reserve(_shaders.size());
    for (const auto& shader : _shaders) shaderStages.push_back(shader.get_stage_create_info());

	GraphicsPipelineCreateInfo createInfo = {
		shaderStages, // create shaders
		{	// describe how vertices are inputed into shaders
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			nullptr,
			0,
			1,
			&Vertex::get_binding_description(),
			static_cast<uint32>(Vertex::get_attribute_descriptions().size()),
			Vertex::get_attribute_descriptions().data()
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
			(float) info.size.width,
			(float) info.size.height,
			0.0f,
			1.0f
		},
		{	// define where you will acutally draw to
			{0, 0},
			info.area
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
			static_cast<VkPolygonMode>(info.renderMode),
			static_cast<VkCullModeFlags>(info.culling),
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
			Application::context()->swapchain()->colorResources()->maxSamples(),
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
			static_cast<uint32>(createInfo.dynamicStates.size()),
			createInfo.dynamicStates.data()
		}
	};

	// create the layout
	create_layout();

	// create the pipeline
	VkGraphicsPipelineCreateInfo pipelineInfo = {
		VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,	
		nullptr,
		0,
		static_cast<uint32>(createInfo.shaderStages.size()),
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
		_layout,
		info.camera->framebuffers()->renderPass(),
		0,
		VK_NULL_HANDLE,
		0
	};

	lassert(vkCreateGraphicsPipelines(Application::context()->device()->device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_pipeline) == VK_SUCCESS, "Failed to create Vulkan Pipeline!");
}

} // namespace lyra
