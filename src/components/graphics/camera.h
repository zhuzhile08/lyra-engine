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
		CameraData();

		glm::mat4 view;
		glm::mat4 proj;
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
	 * @brief rotate the camera around an certain axis
	 *
	 * @param deg degrees of rotation in degrees
	 * @param axis axis to rotate around
	 */
	void rotate(const float deg, const glm::vec3 axis) noexcept {
		_updateQueue.add([&]() { _data.view += glm::rotate(glm::mat4(1.0f), FPS() * deg, axis); });
	}

	/**
	 * @brief set the rotation of the camera around an axis
	 *
	 * @param deg degrees of rotation in degrees
	 * @param axis axis to set the rotation of
	 */
	void set_rotation(const float deg, const glm::vec3 axis) noexcept {
		_updateQueue.add([&]() { _data.view = glm::rotate(glm::mat4(1.0f), FPS() * deg, axis); });
	}
	/**
	 * @brief move the camera by a certain amount
	 *
	 * @param pos amount to move the camera
	 */
	void move(const glm::vec3 pos) noexcept {
		_updateQueue.add([&]() {
			_data.view += glm::translate(glm::mat4(1.0f), pos);
		});
	}
	/**
	 * @brief set the position of the camera
	 *
	 * @param pos position to set to
	 */
	void set_position(const glm::vec3 pos) noexcept {
		_updateQueue.add([&]() {
			_data.view = glm::translate(glm::mat4(1.0f), pos);
		});
	}
	/**
	 * @brief make the camera look at a position
	 *
	 * @param target target to look at
	 * @param up normalized camera direction
	 */
	void look_at(const glm::vec3 target, const glm::vec3 up = { 0.0f, 0.0f, 0.1f }) noexcept {
		_updateQueue.add([&]() { _data.view = glm::lookAt({ 2.0f, 2.0f, 2.0f }, target, up); });
	}
	/**
	 * @brief set the perspective of the camera
	 *
	 * @param fov field of view
	 * @param near near clipping plane
	 * @param far far clipping plane
	 */
	void set_perspective(const float fov = Settings::Rendering::fov, const float near = 0.1f, const float far = 200.0f) noexcept {
		_updateQueue.add([&]() { _data.proj = glm::perspective(Settings::Rendering::fov, _aspect, 0.1f, 200.0f); _data.proj[1][1] *= -1; });
	}

	/**
	 * @brief temporary draw function
	*/
	void draw() {
		CameraData dat {};
		dat.view = glm::translate(glm::mat4(1.0f), { 0.f,-6.f,-10.f });
		dat.proj = glm::perspective(Settings::Rendering::fov, _aspect, 0.1f, 200.0f); 
		dat.proj[1][1] *= -1;

		_buffers[Application::context()->currentFrame()].copy_data(&dat);

		_data = dat;
	}

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
	/**
	 * @brief get the aspect ratio of the camera
	 * 
	 * @return const float
	*/
	[[nodiscard]] const float aspect() const noexcept { return _aspect; }

private:
	std::vector<VulkanGPUBuffer> _buffers;
	CameraData _data;
	CallQueue _updateQueue;
	
	float _aspect = 0;
};

} // namespace lyra
