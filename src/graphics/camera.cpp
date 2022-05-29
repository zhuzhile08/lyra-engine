#include <graphics/camera.h>

namespace lyra {

Camera::Camera() {
	Logger::log_info("Creating Camera... ");

	// create the buffers
	_buffers.resize(Settings::Rendering::maxFramesInFlight);

	for (auto& buffer : _buffers) buffer.create(Application::context()->device(), sizeof(CameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

	Logger::log_info("Successfully created Camera at ", get_address(this), "!", Logger::end_l());
}

void Camera::set_perspective(float fov, float width, float height, float near, float far) noexcept {
	_updateQueue.add([&]() {
		_data.fov = fov;
		_data.width = width;
		_data.height = height;
		_data.near = near;
		_data.far = far;
	});
}

void Camera::look_at(const glm::vec3 center) noexcept {
	_updateQueue.add([&]() {
		const glm::vec3 distance = _data.position - center;
		_data.rotation.z = 0;
		_data.rotation.y = 0;
		_data.rotation.x = 0;
	});
}

void Camera::draw() {
	_updateQueue.flush();

	_buffers[Application::context()->currentFrame()].copy_data(&_data);

	_data = CameraData();
}

} // namespace lyra
