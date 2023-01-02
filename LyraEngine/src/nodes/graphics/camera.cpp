#include <nodes/graphics/camera.h>

#include <gtc/matrix_transform.hpp>

#include <math/math.h>

#include <nodes/script.h>

#include <rendering/vulkan/vulkan_shader.h>
#include <rendering/vulkan/vulkan_pipeline.h>
#include <rendering/graphics_pipeline.h>
#include <rendering/vulkan/descriptor.h>
#include <rendering/material.h>
#include <rendering/vulkan/vulkan_window.h>

#include <nodes/graphics/cubemap.h>

#include <core/application.h>

namespace lyra {

Camera::Camera(
	Script* script,
	const bool& perspective,
	const char* name,
	Spatial* parent,
	const bool& visible,
	const uint32& tag,
	const Transform& transform
) :
	Spatial(script, name, parent, visible, tag, transform), Renderer(), m_projection_matrix(glm::mat4(1.0f))
{
	// shader information
	std::vector<vulkan::Pipeline::ShaderInfo> shaders{
		{ vulkan::Shader::Type::TYPE_VERTEX, "data/shader/vert.spv", "main" },
		{ vulkan::Shader::Type::TYPE_FRAGMENT, "data/shader/frag.spv", "main" }
	};

	// binding information
	std::vector<vulkan::Pipeline::Binding> bindings{
		{ vulkan::Shader::Type::TYPE_VERTEX, 0, vulkan::Descriptor::Type::TYPE_UNIFORM_BUFFER, Settings::Rendering::maxFramesInFlight },
		{ vulkan::Shader::Type::TYPE_VERTEX, 1, vulkan::Descriptor::Type::TYPE_UNIFORM_BUFFER, Settings::Rendering::maxFramesInFlight },
		{ vulkan::Shader::Type::TYPE_FRAGMENT, 1, vulkan::Descriptor::Type::TYPE_IMAGE_SAMPLER, Settings::Rendering::maxFramesInFlight},
		{ vulkan::Shader::Type::TYPE_VERTEX, 1, vulkan::Descriptor::Type::TYPE_IMAGE_SAMPLER, Settings::Rendering::maxFramesInFlight },
		{ vulkan::Shader::Type::TYPE_VERTEX, 1, vulkan::Descriptor::Type::TYPE_IMAGE_SAMPLER, Settings::Rendering::maxFramesInFlight },
		{ vulkan::Shader::Type::TYPE_FRAGMENT, 1, vulkan::Descriptor::Type::TYPE_UNIFORM_BUFFER, Settings::Rendering::maxFramesInFlight },
		{ vulkan::Shader::Type::TYPE_FRAGMENT, 1, vulkan::Descriptor::Type::TYPE_IMAGE_SAMPLER, Settings::Rendering::maxFramesInFlight },
		{ vulkan::Shader::Type::TYPE_FRAGMENT, 1, vulkan::Descriptor::Type::TYPE_IMAGE_SAMPLER, Settings::Rendering::maxFramesInFlight },
		{ vulkan::Shader::Type::TYPE_FRAGMENT, 1, vulkan::Descriptor::Type::TYPE_IMAGE_SAMPLER, Settings::Rendering::maxFramesInFlight }
	};

	// create the graphics pipeline
	m_renderPipeline = SmartPointer<GraphicsPipeline>::create(
		this,
		shaders,
		bindings,
		std::vector<VkPushConstantRange> {}
		);

	// preallocate the memory for the buffer that sends the camera data to the shaders
	m_buffers.reserve(Settings::Rendering::maxFramesInFlight);
	for (uint32 i = 0; i < Settings::Rendering::maxFramesInFlight; i++) { 
		// create the buffers that send the camera information to the shaders and copy in the information
		m_buffers.emplace_back(sizeof(CameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
	}

	// create the descriptor sets
	vulkan::Descriptor::Writer writer;
	writer.add_writes({ // write the buffers
		{ m_buffers[0].get_descriptor_buffer_info(), 0, lyra::vulkan::Descriptor::Type::TYPE_UNIFORM_BUFFER },
		{ m_buffers[1].get_descriptor_buffer_info(), 0, lyra::vulkan::Descriptor::Type::TYPE_UNIFORM_BUFFER }
		});

	// create the descriptors themselves
	m_descriptors.reserve(Settings::Rendering::maxFramesInFlight);
	for (uint32 i = 0; i < Settings::Rendering::maxFramesInFlight; i++) 
		m_descriptors.emplace_back(
			m_renderPipeline->descriptorSetLayout(), 
			0,
			m_renderPipeline->descriptorPool(), 
			writer
		);

	// automatically set camera mode to perspective
	if (perspective) 
		set_perspective();
	else 
		set_orthographic();
}

void Camera::recreate() {
	for (auto framebuffer : m_framebuffers) vkDestroyFramebuffer(Application::renderSystem.device.device(), framebuffer, nullptr);
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
	/**
	m_fov = fov;
	m_near = near;
	m_far = far;
	m_projection_matrix = glm::perspective(glm::radians(fov), Application::renderSystem.vulkanWindow.extent().width/(float)Application::renderSystem.vulkanWindow.extent().height, m_near, m_far); 
	*/
	m_projection_matrix = perspective_matrix(
		m_fov,  
		Application::renderSystem.vulkanWindow.extent().width,
		Application::renderSystem.vulkanWindow.extent().height,
		m_near,
		m_far
	);
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
	//m_script->update();
	// check wich projection model the camera uses and calculate the projection data
	//CameraData data {mat_to_global(), m_projection_matrix};

	glm::mat4 model = glm::mat4(1.0f), view = glm::mat4(1.0f), proj;
	model = glm::rotate(glm::mat4(1.0f), glm::radians(3.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    proj = glm::perspective(glm::radians(45.0f), Application::renderSystem.vulkanWindow.extent().width/(float)Application::renderSystem.vulkanWindow.extent().height, 0.1f, 10.0f);

	CameraData data {model * view, proj};

	// copy the data into the shader
	m_buffers[Application::renderSystem.currentFrame()].copy_data(&data);
}

void Camera::record_command_buffers() {
	// calculate the camera matrices
	draw();
	// begin the renderpass
	begin_renderpass();
	// draw the skybox first as background
	if (m_skybox != nullptr) m_skybox->draw();
	// bind the default render pipeline
	Application::renderSystem.currentCommandBuffer.bindPipeline(m_renderPipeline->bindPoint(), m_renderPipeline->pipeline());
	Application::renderSystem.currentCommandBuffer.bindDescriptorSet(
		m_renderPipeline->bindPoint(), 
		m_renderPipeline->layout(),
		0, 
		m_descriptors[Application::renderSystem.currentFrame()].get());
	// loop through the materials and draw their meshes
	for (uint32 i = 0; i < m_materials.size(); i++) m_materials.at(i)->draw();
	// end renderpass
	end_renderpass();
}

} // namespace lyra
