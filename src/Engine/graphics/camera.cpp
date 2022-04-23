#include <graphics/camera.h>

namespace lyra {

Camera::CameraData::CameraData() : model(glm::mat4(1.0f)), view(glm::mat4(1.0f)), proj(glm::mat4(1.0f)) { }

Camera::Camera() { }

Camera::~Camera() noexcept {
	for (auto& buffer : _buffers) buffer.destroy();

	LOG_INFO("Succesfully destroyed Vulkan uniform buffers!");
}

void Camera::destroy() noexcept {
	this->~Camera();
}

void Camera::create(const Context* context) {
	this->context = context;

	// create the buffers
	_buffers.resize(MAX_FRAMES_IN_FLIGHT);

	for (auto& buffer : _buffers) buffer.create(&context->device(), sizeof(CameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_TO_CPU);

	_aspect = context->swapchain().extent().width / (float)context->swapchain().extent().height;

	LOG_INFO("Succesfully created Vulkan uniform buffers at ", GET_ADDRESS(this), "!", END_L);
}

void Camera::rotate(const float deg, const glm::vec3 axis) noexcept {
	_updateQueue.add([&]() { _data.model += glm::rotate(glm::mat4(1.0f), FPS() * glm::radians(deg), axis); });
}

void Camera::set_rotation(const float deg, const glm::vec3 axis) noexcept {
	_updateQueue.add([=]() { _data.model = glm::rotate(glm::mat4(1.0f), FPS() * glm::radians(deg), axis); });
}

void Camera::move(const glm::vec3 pos) noexcept {
	_updateQueue.add([&]() { 
		_position += pos;
		_data.view += glm::translate(glm::mat4(1.0f), pos); 
	});
}

void Camera::set_position(const glm::vec3 pos) noexcept {
	_updateQueue.add([&]() {
		_position = pos;
		_data.view = glm::translate(glm::mat4(1.0f), pos);
	});
}

void Camera::set_perspective(const float aspect, const float fov, const float near, const float far) noexcept {
	_updateQueue.add([&]() { _data.proj = glm::perspective(glm::radians(fov), aspect, near, far); });
}

void Camera::look_at(const glm::vec3 target, const glm::vec3 up) noexcept {
	_updateQueue.add([&]() { _data.view = glm::lookAt(_position, target, up); });
}

void Camera::draw() {
	_updateQueue.flush();
	/*
	_data.view = glm::lookAt(_position, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.1f });
	_data.proj = glm::perspective(glm::radians(static_cast<float>(FOV)), _aspect, 0.1f, 10.0f);
	*/

	_data = CameraData();

	_data.model = glm::rotate(glm::mat4(1.0f), FPS() * glm::radians(90.0f), { 0, 0, 1 });

	_buffers[context->currentFrame()].copy_data(&_data);

	_data = CameraData();
}

} // namespace lyra
