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
#include <components/gameObj.h>
#include <core/rendering/vulkan/GPU_buffer.h>
#include <core/rendering/renderer.h>
#include <core/settings.h>
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
class Camera : public Renderer {
public:
	// projection mode of the camera
	enum Projection {
		PROJECTION_PERSPECTIVE,
		PROJECTION_ORTHOGRAPHIC
	};

	// projection mode of the camera
	enum Clear {
		CLEAR_SKYBOX,
		CLEAR_COLOR,
		CLEAR_NONE
	};

	// camera data
	struct CameraData {
		CameraData() { }

		glm::mat4 model;
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
	 * @brief set the projection of the camera in perspective mode
	 * 
	 * @param fov field of view
	 * @param near near clipping plane
	 * @param far far clipping plane
	*/
	void set_perspective(float fov = 45.0f, float near = 0.1f, float far = 20.0f) noexcept;
	/**
	 * @brief set the projection of the camera in orthographic mode
	 *
	 * @param viewport viewport
	 * @param near near clipping plane
	 * @param far far clipping plane
	*/
	void set_orthographic(glm::vec4 viewport = { 0.0f, 0.0f, 1.0f, 1.0f }, float near = 0.1f, float far = 20.0f) noexcept;

	/**
	 * @brief draw function
	*/
	void draw(CameraData data);

	/**
	 * @brief get the GPU memory buffers
	 * 
	 * @return const std::vector<VulkanGPUBuffer>&
	*/
	[[nodiscard]] const std::vector<VulkanGPUBuffer>& buffers() const noexcept { return _buffers; }
	/**
	 * @brief get the field of view of the camera
	 *
	 * @return const float
	*/
	[[nodiscard]] const float fov() const noexcept { return _fov; }
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
	/**
	 * @brief get the viewport dimensions and positions
	 *
	 * @return const float
	*/
	[[nodiscard]] const glm::vec4 viewport() const noexcept { return _viewport; }

private:
	std::vector<VulkanGPUBuffer> _buffers;

	float _fov = 45.0f, _near = 0.1f, _far = 20.0f, _depth = 1.0f;
	glm::vec4 _viewport = { 0.0f, 0.0f, 1.0f, 1.0f };
	Projection _projection;
};

} // namespace lyra
