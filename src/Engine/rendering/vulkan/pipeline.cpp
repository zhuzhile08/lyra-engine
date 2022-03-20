#include <rendering/vulkan/pipeline.h>

namespace lyra {

VulkanGraphicsPipeline::VulkanGraphicsPipeline() { }

void VulkanGraphicsPipeline::destroy() {
	vkDestroyPipeline(device->get().device, var.graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device->get().device, var.pipelineLayout, nullptr);
	for (auto& shader : var.shaders) shader.destroy();

    delete device;

	LOG_INFO("Succesfully destroyed Vulkan graphics pipeline!")
}

void VulkanGraphicsPipeline::create(
	VulkanDevice                            device,    
	const VulkanFramebuffers                framebuffer, 
	const VulkanDescriptorSetLayout         descriptorSetLayout,
	const std::vector<ShaderCreationInfo>   shaderCreationInfos,
	VkExtent2D                              size,
    VkExtent2D                              area
) {
	LOG_INFO("Creating Vulkan graphics pipeline...")
	
    this->device = &device;

    var.shaders.reserve(shaderCreationInfos.size());

	create_shaders(shaderCreationInfos);

	create_pipeline(framebuffer, descriptorSetLayout, size, area);

	LOG_INFO("Succesfully created Vulkan pipeline at ", GET_ADDRESS(this), "!", END_L)
}

void VulkanGraphicsPipeline::create_pipeline(
	const VulkanFramebuffers        framebuffer, 
	const VulkanDescriptorSetLayout descriptorSetLayout,
	VkExtent2D                      size,
    VkExtent2D                      area
) {
    // add all the shader stage creation information into a vector
    std::vector <VkPipelineShaderStageCreateInfo>   shaderStages;
    shaderStages.reserve(var.shaders.size());
    for (const auto& shader : var.shaders) shaderStages.push_back(shader.get().stage);

	VulkanGraphicsPipelineCreateInfo                createInfo = {
		shaderStages, // create shaders
		{	// describe how vertices are inputed into shaders
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			nullptr,
			0,
			1,
			&Mesh::Vertex::get_binding_description(),
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
			POLYGON_MODE,
			VK_CULL_MODE_BACK_BIT,
			VK_FRONT_FACE_CLOCKWISE,
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
			VK_SAMPLE_COUNT_1_BIT,
			VK_FALSE,				// currently set to false
			0,
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
#if ENABLE_COLOR_BLENDING
			VK_TRUE,
			VK_BLEND_FACTOR_SRC_ALPHA,
			VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			VK_BLEND_OP_ADD,
			VK_BLEND_FACTOR_ONE,
			VK_BLEND_FACTOR_ZERO,
			VK_BLEND_OP_ADD,
#else								// currently set to false
			VK_FALSE,
			VK_BLEND_FACTOR_SRC_ALPHA,
			VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			VK_BLEND_OP_ADD,
			VK_BLEND_FACTOR_ONE,
			VK_BLEND_FACTOR_ZERO,
			VK_BLEND_OP_ADD,
#endif
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
		},
		{
			VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			nullptr,
			0,
#if ENABLE_COLOR_BLENDING
			VK_TRUE,
			VK_LOGIC_OP_COPY,
#else
			VK_FALSE,
			VK_LOGIC_OP_COPY,
#endif
			1,
			&createInfo.colorBlendAttachment,
			{
				0.0f, 0.0f, 0.0f, 0.0f
			}
		},
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
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
	create_layout(descriptorSetLayout);

	// create the pipeline
	VkGraphicsPipelineCreateInfo pipelineInfo = {
		VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,	
		nullptr,
		0,
		ARR_SIZE(createInfo.shaderStages),
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
		var.pipelineLayout,
		framebuffer.get().renderPass,
		0,
		VK_NULL_HANDLE,
		0
	};

	if(vkCreateGraphicsPipelines(device->get().device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &var.graphicsPipeline) != VK_SUCCESS) 
		LOG_EXEPTION("Failed to create Vulkan Pipeline")
}

void VulkanGraphicsPipeline::create_layout(const VulkanDescriptorSetLayout descriptorSetLayout) {
	// create the pipeline layout
	VkPipelineLayoutCreateInfo 				pipelineLayoutInfo {
		VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		nullptr,
		0,
		1,
		descriptorSetLayout.get_ptr(),
		0,
		nullptr
	};

	if(vkCreatePipelineLayout(device->get().device, &pipelineLayoutInfo, nullptr, &var.pipelineLayout) != VK_SUCCESS) LOG_EXEPTION("Failed to create Vulkan graphics pipeline layout");
}

void VulkanGraphicsPipeline::create_shaders(std::vector<ShaderCreationInfo> shaderCreationInfos) {
	if (shaderCreationInfos.size() != var.shaders.size()) {
		LOG_WARNING("Number of shader creation infos doesn't match up with the numbers of shaders in the pipeline at: ", GET_ADDRESS(this), "!")
	}

	for (uint16 index = 0; index <= shaderCreationInfos.size(); index++) {
		var.shaders[index].create(*device, shaderCreationInfos[index].path, shaderCreationInfos[index].entry, shaderCreationInfos[index].flag);
		LOG_DEBUG(TAB, "Succesfully created Vulkan shader at: ", GET_ADDRESS(&var.shaders[index]), " with flag: ", shaderCreationInfos[index].flag, "!")
	}
}

VulkanGraphicsPipeline::Variables VulkanGraphicsPipeline::get() const {
	return var;
}

} // namespace lyra
