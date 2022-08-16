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
	
	// create the graphics pipeline
	std::vector<VulkanPipeline::Binding> bindings{
		{ lyra::VulkanDescriptor::Type::TYPE_UNIFORM_BUFFER, 1, lyra::Settings::Rendering::maxFramesInFlight, lyra::VulkanShader::Type::TYPE_VERTEX, "data/shader/vert.spv", "main" },
		{ lyra::VulkanDescriptor::Type::TYPE_IMAGE_SAMPLER, 1, lyra::Settings::Rendering::maxFramesInFlight, lyra::VulkanShader::Type::TYPE_FRAGMENT, "data/shader/frag.spv", "main" }
	};

	_renderPipeline = SmartPointer<GraphicsPipeline>::create(
		this,
		bindings,
		lyra::Application::renderSystem()->vulkanWindow()->extent(),
		lyra::Application::renderSystem()->vulkanWindow()->extent()
		);

	// create the buffers
	_buffers.reserve(Settings::Rendering::maxFramesInFlight);
	for (uint32 i = 0; i < Settings::Rendering::maxFramesInFlight; i++) _buffers.emplace_back(sizeof(CameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

	// add the update and draw functions into the queues
	Logger::log_info("Successfully created Camera at ", get_address(this), "!", Logger::end_l());
}

void Camera::set_perspective(float fov, float near, float far) noexcept {
	_projection = Projection::PROJECTION_PERSPECTIVE;
	_fov = fov;
	_near = near;
	_far = far;
}

void Camera::set_orthographic(glm::vec4 viewport, float near, float far) noexcept {
	_projection = Projection::PROJECTION_ORTHOGRAPHIC;
	_viewport = viewport;
	_near = near;
	_far = far;
}

void Camera::draw(CameraData data) {
	if (_projection == Projection::PROJECTION_PERSPECTIVE) data.proj = glm::perspective(glm::radians(_fov), Settings::Window::width / (float) Settings::Window::height, _near, _far);
	else data.proj = glm::ortho(_viewport[0], _viewport[1] + _viewport[0], _viewport[2], _viewport[3] + _viewport[2], _near, _far);
	data.proj[1][1] *= -1;

	_buffers.at(Application::renderSystem()->currentFrame()).copy_data(&data);
}

void Camera::record_command_buffers() const {
	begin_renderpass();

	vkCmdBindPipeline(Application::renderSystem()->activeCommandBuffer(), _renderPipeline->bindPoint(), _renderPipeline->pipeline());

	for (int i = 0; i < _materials.size(); i++) _materials.at(i)->draw();

	end_renderpass();
}

} // namespace lyra
