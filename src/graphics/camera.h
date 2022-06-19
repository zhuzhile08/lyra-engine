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

		glm::mat4 model;
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
	 * @brief set the perspective of the camera
	 * 
	 * @param fov field of view
	 * @param width width of the camera
	 * @param height height of the camera
	 * @param near near clipping plane
	 * @param far far clipping plane
	*/
	void set_perspective(float fov = 45.0f, float width = Settings::Window::width, float height = Settings::Window::height, float near = 0.1f, float far = 20.0f) noexcept;

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
	 * @brief get the field of view of the camera
	 *
	 * @return const float
	*/
	[[nodiscard]] const float fov() const noexcept { return _fov; }
	/**
	 * @brief get the width of the camera
	 *
	 * @return const float
	*/
	[[nodiscard]] const float width() const noexcept { return _width; }
	/**
	 * @brief get the height of the camera
	 *
	 * @return const float
	*/
	[[nodiscard]] const float height() const noexcept { return _height; }
	/**
	 * @brief get the near clipping plane of the camera
	 *
	 * @return const float
	*/
	[[nodiscard]] const float near() const noexcept { return _near; }
	/**
	 * @brief get the far clipping plane of the camera
	 *
	 * @return const float
	*/
	[[nodiscard]] const float far() const noexcept { return _far; }

private:
	std::vector<VulkanGPUBuffer> _buffers;
	CameraData _data;
	float _fov = 45.0f, _width = Settings::Window::width, _height = Settings::Window::height, _near = 0.1f, _far = 20.0f;
};

} // namespace lyra
