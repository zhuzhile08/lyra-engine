#include <components/graphics/camera.h>

namespace lyra {

Camera::Camera() {
	Logger::log_info("Creating Camera... ");

	// create the buffers
	_buffers.resize(Settings::Rendering::maxFramesInFlight);
	for (auto& buffer : _buffers) buffer.create(Application::context()->device(), sizeof(CameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

	// bind the renderer
	bind();

	// add the update and draw functions into the queues
	Logger::log_info("Successfully created Camera at ", get_address(this), "!", Logger::end_l());
}

void Camera::set_perspective(float fov, float near, float far) noexcept {
	_fov = fov;
	_near = near;
	_far = far;
}

void Camera::set_orthographic(glm::vec4 viewport, float near, float far) noexcept {
	_viewport = viewport;
	_near = near;
	_far = far;
}

void Camera::draw(CameraData data) {
	if (_projection = Projection::PROJECTION_PERSPECTIVE) data.proj = glm::perspective(glm::radians(_fov), Settings::Window::width / (float) Settings::Window::height, _near, _far);
	else data.proj = glm::ortho(_viewport[0], _viewport[1] + _viewport[0], _viewport[2], _viewport[3] + _viewport[2], _near, _far);
	data.proj[1][1] *= -1;

	_buffers.at(Application::context()->currentFrame()).copy_data(&data);
}

} // namespace lyra
