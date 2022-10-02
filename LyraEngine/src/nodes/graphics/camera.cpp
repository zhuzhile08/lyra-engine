#include <nodes/graphics/camera.h>

#include <core/logger.h>
#include <core/util.h>

#include <gtc/matrix_transform.hpp>

#include <core/application.h>
#include <core/rendering/vulkan/vulkan_window.h>
#include <core/rendering/vulkan/vulkan_pipeline.h>
#include <core/rendering/vulkan/vulkan_shader.h>
#include <core/rendering/vulkan/descriptor.h>
#include <core/rendering/graphics_pipeline.h>
#include <core/rendering/material.h>

namespace lyra {

Camera::Camera(const char* name, Spatial* parent, const bool visible, const uint32 tag, const glm::vec3 position, const glm::vec3 rotation, const glm::vec3 scale, const RotationOrder rotationOrder) :
	Spatial(name, parent, visible, tag, position, rotation, scale, rotationOrder), Renderer()
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
		{ vulkan::Descriptor::Type::TYPE_IMAGE_SAMPLER, 1, Settings::Rendering::maxFramesInFlight, vulkan::Shader::Type::TYPE_VERTEX },
		{ vulkan::Descriptor::Type::TYPE_IMAGE_SAMPLER, 1, Settings::Rendering::maxFramesInFlight, vulkan::Shader::Type::TYPE_VERTEX },
		{ vulkan::Descriptor::Type::TYPE_UNIFORM_BUFFER, 1, Settings::Rendering::maxFramesInFlight, vulkan::Shader::Type::TYPE_FRAGMENT },
		{ vulkan::Descriptor::Type::TYPE_IMAGE_SAMPLER, 1, Settings::Rendering::maxFramesInFlight, vulkan::Shader::Type::TYPE_FRAGMENT },
		{ vulkan::Descriptor::Type::TYPE_IMAGE_SAMPLER, 1, Settings::Rendering::maxFramesInFlight, vulkan::Shader::Type::TYPE_FRAGMENT },
		{ vulkan::Descriptor::Type::TYPE_IMAGE_SAMPLER, 1, Settings::Rendering::maxFramesInFlight, vulkan::Shader::Type::TYPE_FRAGMENT },
		{ vulkan::Descriptor::Type::TYPE_IMAGE_SAMPLER, 1,Settings::Rendering::maxFramesInFlight, vulkan::Shader::Type::TYPE_FRAGMENT }
	};

	// push constants
	std::vector<VkPushConstantRange> pushConstants {
		{ VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(CameraData) }
	};

	// create the graphics pipeline
	m_renderPipeline = SmartPointer<GraphicsPipeline>::create(
		this,
		shaders,
		bindings,
		pushConstants,
		Application::renderSystem()->vulkanWindow()->extent(),
		Application::renderSystem()->vulkanWindow()->extent()
		);

	// add the update and draw functions into the queues
	Logger::log_info("Successfully created Camera at ", get_address(this), "!", Logger::end_l());
}

void Camera::set_perspective(float fov, float near, float far) noexcept {
	m_projection = Projection::PROJECTION_PERSPECTIVE;
	m_fov = fov;
	m_near = near;
	m_far = far;
}

void Camera::set_orthographic(glm::vec4 viewport, float near, float far) noexcept {
	m_projection = Projection::PROJECTION_ORTHOGRAPHIC;
	m_viewport = viewport;
	m_near = near;
	m_far = far;
}

void Camera::draw() const {
	// check wich projection model the camera uses and calculate the projection data
	CameraData data;
	data.model = mat_to_global();
	if (m_projection == Projection::PROJECTION_PERSPECTIVE) data.proj = glm::perspective(glm::radians(m_fov), Settings::Window::width / (float) Settings::Window::height, m_near, m_far);
	else data.proj = glm::ortho(m_viewport[0], m_viewport[1] + m_viewport[0], m_viewport[2], m_viewport[3] + m_viewport[2], m_near, m_far);
	data.proj[1][1] *= -1;

	Application::renderSystem()->currentCommandBuffer().pushConstants(m_renderPipeline->layout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(CameraData), &data);
}

void Camera::record_command_buffers() const {
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
