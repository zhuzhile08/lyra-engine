#include <nodes/graphics/camera.h>

#include <core/logger.h>
#include <core/util.h>

#include <gtc/matrix_transform.hpp>

namespace lyra {

Camera::Camera(const char* name, Spatial* parent, const bool visible, const uint32 tag, const glm::vec3 position, const glm::vec3 rotation, const glm::vec3 scale, const RotationOrder rotationOrder) :
	Spatial(name, parent, visible, tag, position, rotation, scale, rotationOrder), Renderer()
{
	Logger::log_info("Creating Camera... ");

	// create the buffers
	_buffers.resize(Settings::Rendering::maxFramesInFlight);
	for (uint32 i = 0; i < _buffers.size(); i++) _buffers.emplace_back(Context::get()->renderSystem()->device(), sizeof(CameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

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

	_buffers.at(Context::get()->renderSystem()->currentFrame()).copy_data(&data);
}

} // namespace lyra
