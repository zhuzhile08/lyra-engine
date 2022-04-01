#include <rendering/camera.h>

namespace lyra {

Camera::Camera() { }

void Camera::destroy() noexcept {
	for (auto buffer : _buffers) buffer.destroy();

	LOG_INFO("Succesfully destroyed Vulkan uniform buffers!")
}

void Camera::create(const Renderer* renderer, const UniformBufferObject&& ubo) {
	this->renderer = renderer;
	_ubo = ubo;

	// create the buffers
	_buffers.resize(MAX_FRAMES_IN_FLIGHT);

	for (auto buffer : _buffers) buffer.create(&renderer->device(), sizeof(UniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_TO_CPU);

	set_perspective(renderer->swapchain().extent().width / (float)renderer->swapchain().extent().height);

	VulkanDescriptor::Writer writer;
	for(auto& buffer : _buffers) writer.add_buffer_write(buffer.get_descriptor_buffer_info(), 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

	_descriptor.create(&renderer->device(), renderer->descriptorSetLayout(), renderer->descriptorPool(), writer);

	LOG_INFO("Succesfully created Vulkan uniform buffers at ", GET_ADDRESS(this), "!", END_L)
}

void Camera::rotate(const float deg, const glm::vec3 axis) noexcept {
	_update_queue.add([&]() { _ubo.model += glm::rotate(glm::mat4(1.0f), FPS() * glm::radians(deg), axis); });
}

void Camera::set_rotation(const float deg, const glm::vec3 axis) noexcept {
	_update_queue.add([&]() { _ubo.model = glm::rotate(glm::mat4(1.0f), FPS() * glm::radians(deg), axis); });
}

void Camera::move(const glm::vec3 pos) noexcept {
	_update_queue.add([&]() { 
		_position += pos;
		_ubo.view += glm::translate(glm::mat4(1.0f), pos); 
	});
}

void Camera::set_position(const glm::vec3 pos) noexcept {
	_update_queue.add([&]() {
		_position = pos;
		_ubo.view = glm::translate(glm::mat4(1.0f), pos);
	});
}

void Camera::set_perspective(const float aspect, const float fov, const float near, const float far) noexcept {
	_update_queue.add([&]() { _ubo.proj = glm::perspective(glm::radians(45.0f), aspect, near, far); });
}

void Camera::look_at(const glm::vec3 target, const glm::vec3 up) noexcept {
	_update_queue.add([&]() { _ubo.view = glm::lookAt(_position, target, up); });
}

void Camera::draw() {
	_update_queue.flush();

	_buffers[renderer->currentFrame()].copy_data(&_ubo);
}

} // namespace lyra
