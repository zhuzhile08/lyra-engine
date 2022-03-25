#include <rendering/camera.h>

namespace lyra {

Camera::Camera() { }

void Camera::destroy() noexcept {
    for(auto buffer : var.buffers) buffer.destroy();

    LOG_INFO("Succesfully destroyed Vulkan uniform buffers!")
}

void Camera::create(VulkanDevice device, const UniformBufferObject && ubo, const VulkanSwapchain swapchain) {
    this->device = new VulkanDevice(device);
    var.ubo = ubo;

    // create the buffers
    var.buffers.resize(MAX_FRAMES_IN_FLIGHT);
    for(auto buffer : var.buffers) buffer.create(device, sizeof(UniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
        VMA_MEMORY_USAGE_GPU_TO_CPU);

    set_perspective(swapchain.get().extent.width / (float) swapchain.get().extent.height);

    LOG_INFO("Succesfully created Vulkan uniform buffers at ", GET_ADDRESS(this), "!", END_L)
}

void Camera::rotate(const float deg, const glm::vec3 axis) noexcept {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    var.ubo.model += glm::rotate(glm::mat4(1.0f), time * glm::radians(deg), axis);
}

void Camera::set_rotation(const float deg, const glm::vec3 axis) noexcept {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    var.ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(deg), axis);
}

void Camera::move(const glm::vec3 pos) noexcept {
    var.position += pos;
    var.ubo.view += glm::translate(glm::mat4(1.0f), pos);
}

void Camera::set_position(const glm::vec3 pos) noexcept {
    var.position = pos;
    var.ubo.view = glm::translate(glm::mat4(1.0f), pos);
}

void Camera::set_perspective(const float aspect, const float fov, const float near, const float far) noexcept {
    var.ubo.proj = glm::perspective(glm::radians(45.0f), aspect, near, far);
} 

void Camera::look_at(const glm::vec3 target, const glm::vec3 up) noexcept {
    var.ubo.view = glm::lookAt(var.position, target, up);
}

void Camera::finish(uint32 index) {
    var.buffers[index].copy_data(&var.ubo);
}

Camera::Variables Camera::get() noexcept {
    return var;
}

} // namespace lyra
