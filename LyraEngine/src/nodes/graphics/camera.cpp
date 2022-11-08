#include <nodes/graphics/camera.h>

#include <gtc/matrix_transform.hpp>

#include <nodes/script.h>

#include <rendering/vulkan/vulkan_shader.h>
#include <rendering/vulkan/vulkan_pipeline.h>
#include <rendering/graphics_pipeline.h>
#include <rendering/vulkan/descriptor.h>
#include <rendering/material.h>
#include <rendering/vulkan/vulkan_window.h>

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
	Spatial(script, name, parent, visible, tag, transform), Renderer()
{
	Logger::log_info("Creating Camera... ");
	
	// shader information
	std::vector<vulkan::Pipeline::ShaderInfo> shaders{
		{ vulkan::Shader::Type::TYPE_VERTEX, "data/shader/vert.spv", "main" },
		{ vulkan::Shader::Type::TYPE_FRAGMENT, "data/shader/frag.spv", "main" }
	};

	// binding information
	std::vector<vulkan::Pipeline::Binding> bindings{
		{ vulkan::Descriptor::Type::TYPE_UNIFORM_BUFFER, 1, Settings::Rendering::maxFramesInFlight, vulkan::Shader::Type::TYPE_VERTEX },
		// { vulkan::Descriptor::Type::TYPE_UNIFORM_BUFFER, 1, Settings::Rendering::maxFramesInFlight, vulkan::Shader::Type::TYPE_VERTEX },
		// { vulkan::Descriptor::Type::TYPE_IMAGE_SAMPLER, 1, Settings::Rendering::maxFramesInFlight, vulkan::Shader::Type::TYPE_VERTEX },
		// { vulkan::Descriptor::Type::TYPE_IMAGE_SAMPLER, 1, Settings::Rendering::maxFramesInFlight, vulkan::Shader::Type::TYPE_VERTEX },
		// { vulkan::Descriptor::Type::TYPE_UNIFORM_BUFFER, 1, Settings::Rendering::maxFramesInFlight, vulkan::Shader::Type::TYPE_FRAGMENT },
		{ vulkan::Descriptor::Type::TYPE_IMAGE_SAMPLER, 1, Settings::Rendering::maxFramesInFlight, vulkan::Shader::Type::TYPE_FRAGMENT }
		// { vulkan::Descriptor::Type::TYPE_IMAGE_SAMPLER, 1, Settings::Rendering::maxFramesInFlight, vulkan::Shader::Type::TYPE_FRAGMENT },
		// { vulkan::Descriptor::Type::TYPE_IMAGE_SAMPLER, 1, Settings::Rendering::maxFramesInFlight, vulkan::Shader::Type::TYPE_FRAGMENT },
		// { vulkan::Descriptor::Type::TYPE_IMAGE_SAMPLER, 1,Settings::Rendering::maxFramesInFlight, vulkan::Shader::Type::TYPE_FRAGMENT }
	};

	// create the graphics pipeline
	m_renderPipeline = SmartPointer<GraphicsPipeline>::create(
		this,
		shaders,
		bindings,
		std::vector<VkPushConstantRange> {},
		Application::renderSystem()->vulkanWindow()->extent(),
		Application::renderSystem()->vulkanWindow()->extent()
		);


	// preallocate the memory for the buffer that sends the camera data to the shaders
	m_buffers.reserve(Settings::Rendering::maxFramesInFlight);
	for (uint32 i = 0; i < Settings::Rendering::maxFramesInFlight; i++) { 
		// create the buffers that send the camera information to the shaders and copy in the information
		m_buffers.emplace_back(sizeof(CameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
	}

	// automatically set camera mode to perspective
	if (perspective) 
		set_perspective();
	else 
		set_orthographic();

	// add the update and draw functions into the queues
	Logger::log_info("Successfully created Camera at ", get_address(this), "!", Logger::end_l());
}

void Camera::set_perspective(float fov, float near, float far) noexcept {
	m_projection = Projection::PROJECTION_PERSPECTIVE;
	m_fov = fov;
	m_near = near;
	m_far = far;
	m_projection_matrix = glm::perspective(glm::radians(m_fov), Settings::Window::width / (float) Settings::Window::height, m_near, m_far);
	m_projection_matrix[1][1] *= -1;
}

void Camera::set_orthographic(glm::vec4 viewport, float near, float far) noexcept {
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
	CameraData data {mat_to_global(), m_projection_matrix};
	// copy the data into the shader
	m_buffers.at(Application::renderSystem()->currentFrame()).copy_data(&data);
}

void Camera::record_command_buffers() {
	// calculate the camera matrices
	draw();
	// begin the renderpass
	begin_renderpass();
	// bind the default render pipeline
	Application::renderSystem()->currentCommandBuffer().bindPipeline(m_renderPipeline->bindPoint(), m_renderPipeline->pipeline());
	// loop through the materials and draw their meshes
	for (int i = 0; i < m_materials.size(); i++) m_materials.at(i)->draw();
	// end renderpass
	end_renderpass();
}

} // namespace lyra
