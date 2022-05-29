/*************************
 * @file uniform_buffer.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief uniform buffer objects
 *
 * @date 2022-03-07
 *
 * @copyright Copyright (c) 2022 Zhile Zhu
 *************************/

#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <lyra.h>
#include <core/rendering/vulkan/GPU_buffer.h>
#include <core/logger.h>
#include <core/defines.h>
#include <core/queue_types.h>
#include <math/math.h>

#include <vector>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <vulkan/vulkan.h>

namespace lyra {

/**
 * @brief wrapper arond a uniform buffer object
 */
class Camera {
public:
	struct CameraData {
		CameraData() { }

		glm::vec3 rotation = glm::vec3(1.0f);
		glm::vec3 position = glm::vec3(1.0f);
		float fov = Settings::Rendering::fov; 
		float width = Settings::Window::width; 
		float height = Settings::Window::height; 
		float near = 0.1f; 
		float far = 200.f;
	};

	/**
	 * @brief construct a new camera
	*/
	Camera();

	/**
	 * @brief destroy the camera
	 */
	void destroy() noexcept {
		this->~Camera();
	}

	Camera operator=(const Camera&) const noexcept = delete;

	/**
	 * @brief set the perspective of the camera
	 * 
	 * @param fov FOV
	 * @param width screen width
	 * @param height screen height
	 * @param near near clipping plane
	 * @param far far clipping plane
	*/
	void set_perspective(float fov = Settings::Rendering::fov, float width = Settings::Window::width, float height = Settings::Window::height, float near = 0.1f, float far = 200.f) noexcept;
	/**
	 * @brief set the rotation of the camera around an axis
	 *
	 * @param deg degrees of rotation in degrees
	 * @param axis axis to set the rotation of
	 */
	void set_rotation(const float angle, glm::vec3 up = { 0.0f, 0.0f, 0.1f }) noexcept {
		_updateQueue.add([&]() {
			_data.rotation = up * angle;
		});
	}
	/**
	 * @brief move the camera by a certain amount
	 *
	 * @param pos amount to move the camera
	 */
	void set_position(const glm::vec3 pos) noexcept {
		_updateQueue.add([&]() {
			_data.position = pos;
		});
	}

	/**
	 * @brief look at a position from the camera's current position
	 * 
	 * @param center place to look at
	*/
	void look_at(const glm::vec3 center) noexcept;

	/**
	 * @brief temporary draw function
	*/
	void draw();

	/**
	 * @brief get the GPU memory buffers
	 * 
	 * @return const std::vector<VulkanGPUBuffer>&
	*/
	[[nodiscard]] const std::vector<VulkanGPUBuffer>& buffers() const noexcept { return _buffers; }
	/**
	 * @brief get the camera data of the camera
	 * 
	 * @return const CameraData
	*/
	[[nodiscard]] const CameraData data() const noexcept { return _data; }
	/**
	 * @brief get the queue with the update commands
	 * 
	 * @return const CallQueue
	*/
	[[nodiscard]] const CallQueue& updateQueue() const noexcept { return _updateQueue; }

private:
	std::vector<VulkanGPUBuffer> _buffers;
	CameraData _data;
	CallQueue _updateQueue;
};

} // namespace lyra
