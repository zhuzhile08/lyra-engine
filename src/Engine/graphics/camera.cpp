#include <graphics/camera.h>

namespace lyra {

Camera::CameraData::CameraData() : view(glm::mat4(1.0f)), proj(glm::mat4(1.0f)) { }

Camera::Camera() { }

Camera::~Camera() noexcept {
	LOG_INFO("Succesfully destroyed Vulkan uniform buffers!");
}

void Camera::destroy() noexcept {
	this->~Camera();
}

void Camera::create(const Context* context) {
	LOG_INFO("Creating Camera... ");

	this->context = context;

	// create the buffers
	_buffers.resize(MAX_FRAMES_IN_FLIGHT);

	for (auto& buffer : _buffers) buffer.create(&context->device(), sizeof(CameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

	_aspect = context->swapchain().extent().width / (float)context->swapchain().extent().height;

	LOG_INFO("Succesfully created Camera at ", GET_ADDRESS(this), "!", END_L);
}

void Camera::rotate(const float deg, const glm::vec3 axis) noexcept {
	_updateQueue.add([&]() { _data.view += glm::rotate(glm::mat4(1.0f), FPS() * glm::radians(deg), axis); });
}

void Camera::set_rotation(const float deg, const glm::vec3 axis) noexcept {
	_updateQueue.add([&]() { _data.view = glm::rotate(glm::mat4(1.0f), FPS() * glm::radians(deg), axis); });
}

void Camera::move(const glm::vec3 pos) noexcept {
	_updateQueue.add([&]() { 
		_data.view += glm::translate(glm::mat4(1.0f), pos); 
	});
}

void Camera::set_position(const glm::vec3 pos) noexcept {
	_updateQueue.add([&]() {
		_data.view = glm::translate(glm::mat4(1.0f), pos);
	});
}

void Camera::set_perspective(const float aspect, const float fov, const float near, const float far) noexcept {
	_updateQueue.add([&]() { _data.proj = glm::perspective(glm::radians(fov), aspect, near, far); });
}

void Camera::look_at(const glm::vec3 target, const glm::vec3 up) noexcept {
	_updateQueue.add([&]() { _data.view = glm::lookAt({ 2.0f, 2.0f, 2.0f }, target, up); });
}

void Camera::draw() {
	_updateQueue.flush();

	_buffers[context->currentFrame()].copy_data(&_data);

	_data = CameraData();
}

} // namespace lyra
