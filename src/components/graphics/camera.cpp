#include <components/graphics/camera.h>

namespace lyra {

Camera::CameraData::CameraData() : view(glm::mat4(1.0f)), proj(glm::mat4(1.0f)) { }

Camera::Camera() {
	Logger::log_info("Creating Camera... ");

	// create the buffers
	_buffers.resize(Settings::Rendering::maxFramesInFlight);

	for (auto& buffer : _buffers) buffer.create(Application::context()->device(), sizeof(CameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

	_aspect = Application::context()->swapchain()->extent().width / (float)Application::context()->swapchain()->extent().height;

	Logger::log_info("Successfully created Camera at ", get_address(this), "!", Logger::end_l());
}

} // namespace lyra
