#include <nodes/graphics/camera.h>

#include <glm/gtc/matrix_transform.hpp>

#include <math/math.h>

#include <nodes/script.h>

#include <rendering/vulkan/vulkan_shader.h>
#include <rendering/material.h>
#include <rendering/vulkan/vulkan_window.h>

// #include <nodes/graphics/cubemap.h>

#include <core/application.h>

namespace lyra {

Camera::Camera(
	Script* script,
	Skybox* skybox,
	const bool& perspective,
	const char* name,
	Spatial* parent,
	const bool& visible,
	const uint32& tag,
	const Transform& transform
) :
	Spatial(script, name, parent, visible, tag, transform), Renderer(), m_skybox(skybox), m_projection_matrix(glm::mat4(1.0f))
{
	{
		// the graphics pipeline builder
		GraphicsPipeline::Builder pipelineBuilder(this);
		pipelineBuilder.add_shader_infos({ // shaders
			{ vulkan::Shader::Type::TYPE_VERTEX, "data/shader/vert.spv", "main" },
			{ vulkan::Shader::Type::TYPE_FRAGMENT, "data/shader/frag.spv", "main" }
		});
		pipelineBuilder.add_binding_infos({ // descriptor bindings
			{ vulkan::Shader::Type::TYPE_VERTEX, 0, vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_UNIFORM_BUFFER, Settings::RenderConfig::maxFramesInFlight },
			{ vulkan::Shader::Type::TYPE_VERTEX, 1, vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_UNIFORM_BUFFER, Settings::RenderConfig::maxFramesInFlight },
			{ vulkan::Shader::Type::TYPE_FRAGMENT, 1, vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_IMAGE_SAMPLER, Settings::RenderConfig::maxFramesInFlight},
			{ vulkan::Shader::Type::TYPE_VERTEX, 1, vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_IMAGE_SAMPLER, Settings::RenderConfig::maxFramesInFlight },
			{ vulkan::Shader::Type::TYPE_VERTEX, 1, vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_IMAGE_SAMPLER, Settings::RenderConfig::maxFramesInFlight },
			{ vulkan::Shader::Type::TYPE_FRAGMENT, 1, vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_UNIFORM_BUFFER, Settings::RenderConfig::maxFramesInFlight },
			{ vulkan::Shader::Type::TYPE_FRAGMENT, 1, vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_IMAGE_SAMPLER, Settings::RenderConfig::maxFramesInFlight },
			{ vulkan::Shader::Type::TYPE_FRAGMENT, 1, vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_IMAGE_SAMPLER, Settings::RenderConfig::maxFramesInFlight },
			{ vulkan::Shader::Type::TYPE_FRAGMENT, 1, vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_IMAGE_SAMPLER, Settings::RenderConfig::maxFramesInFlight }
		});
		pipelineBuilder.enable_sample_shading(0.9f); // also enable sample shading

		// create the graphics pipeline
		m_renderPipeline = GraphicsPipeline(pipelineBuilder);
	}


	for (auto& buffer : m_buffers) { 
		// create the buffers that send the camera information to the shaders and copy in the information
		buffer = vulkan::GPUBuffer(sizeof(CameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
	}


	// create the descriptors themselves
	for (auto& descriptorSet : m_descriptorSets) {
		// get a unused descriptor set and push back its pointer
		descriptorSet = m_renderPipeline.descriptorSystem(0).get_unused_set();
		// add the writes
		descriptorSet->add_writes({
			{ m_buffers[0].get_descriptor_buffer_info(), 0, lyra::vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_UNIFORM_BUFFER },
			{ m_buffers[1].get_descriptor_buffer_info(), 0, lyra::vulkan::DescriptorSystem::DescriptorSet::Type::TYPE_UNIFORM_BUFFER }
		});
		// update the descriptor set
		descriptorSet->update();
	}


	// automatically set camera mode to perspective
	if (perspective) 
		set_perspective();
	else 
		set_orthographic();
}

void Camera::recreate() {
	for (const auto& framebuffer : m_framebuffers) vkDestroyFramebuffer(Application::renderSystem.device.device(), framebuffer, nullptr);
	vkDestroyRenderPass(Application::renderSystem.device.device(), m_renderPass, nullptr);
	create_render_pass();
	create_framebuffers();

	switch(m_projection) {
		case Projection::PROJECTION_ORTHOGRAPHIC:
			set_orthographic(m_viewport, m_near, m_far);
		default:
			set_perspective(m_fov, m_near, m_far);
	}
}

void Camera::set_perspective(const float& fov, const float& near, const float& far) noexcept {
	m_projection = Projection::PROJECTION_PERSPECTIVE;
	m_fov = fov;
	m_near = near;
	m_far = far;
	m_projection_matrix = glm::perspective(glm::radians(fov), Application::renderSystem.vulkanWindow.extent().width/(float)Application::renderSystem.vulkanWindow.extent().height, m_near, m_far); 
	m_projection_matrix[1][1] *= -1;
}

void Camera::set_orthographic(const glm::vec4& viewport, const float& near, const float& far) noexcept {
	m_projection = Projection::PROJECTION_ORTHOGRAPHIC;
	m_viewport = viewport;
	m_near = near;
	m_far = far;
	m_projection_matrix = glm::ortho(m_viewport[0], m_viewport[1] + m_viewport[0], m_viewport[2], m_viewport[3] + m_viewport[2], m_near, m_far);
	m_projection_matrix[1][1] *= -1;
}

void Camera::draw() {
	// update the script
	m_script->update();
	// check wich projection model the camera uses and calculate the projection data
	CameraData data { mat_to_global(), m_projection_matrix };
	// copy the data into the shader
	m_buffers[Application::renderSystem.currentFrame()].copy_data(&data);
}

void Camera::record_command_buffers() {
	// calculate the camera matrices
	draw();
	// begin the renderpass
	begin_renderpass();
	// draw the skybox first as background
	// if (m_skybox) m_skybox->draw();
	// bind the default render pipeline
	Application::renderSystem.frames[Application::renderSystem.currentFrame()].commandBuffer().bindPipeline(m_renderPipeline.bindPoint(), m_renderPipeline.pipeline());
	Application::renderSystem.frames[Application::renderSystem.currentFrame()].commandBuffer().bindDescriptorSet(
		m_renderPipeline.bindPoint(), 
		m_renderPipeline.layout(),
		0, 
		*m_descriptorSets[Application::renderSystem.currentFrame()]);
	// loop through the materials and draw their meshes
	for (const auto& material : m_materials) material->draw();
	// end renderpass
	end_renderpass();
}

} // namespace lyra
