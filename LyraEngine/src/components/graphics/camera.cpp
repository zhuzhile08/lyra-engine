#include <components/graphics/camera.h>

namespace lyra {

Camera::Camera() {
	Logger::log_info("Creating Camera... ");

	// create the buffers
	_buffers.resize(Settings::Rendering::maxFramesInFlight);

	for (auto& buffer : _buffers) buffer.create(Application::context()->device(), sizeof(CameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

	Logger::log_info("Successfully created Camera at ", get_address(this), "!", Logger::end_l());
}

void Camera::set_perspective(float fov, float width, float height, float near, float far) noexcept {
	_fov = fov;
	_width = width;
	_height = height;
	_near = near;
	_far = far;
}

void Camera::draw(CameraData data) {
	data.proj = glm::perspective(glm::radians(_fov), _width / _height, _near, _far);
	data.proj[1][1] *= -1;

	_buffers.at(Application::context()->currentFrame()).copy_data(&data);
}

} // namespace lyra
