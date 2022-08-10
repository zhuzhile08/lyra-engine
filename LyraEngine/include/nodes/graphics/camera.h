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

#include <nodes/spatial.h>
#include <core/rendering/vulkan/GPU_buffer.h>
#include <core/rendering/renderer.h>
#include <core/settings.h>

#include <vector>
#include <memory>

#include <glm.hpp>

namespace lyra {

/**
 * @brief wrapper arond a uniform buffer object
 */
class Camera : public Renderer, public Spatial {
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

	Camera() { }
	
	/**
	 * @brief construct a camera node
	 *
	 * @param name name of the object
	 * @param parent parent Node of the object
	 * @param visible visibility of the object
	 * @param tag optional tag of the object
	 * @param position position of the object
	 * @param rotation rotation of the object
	 * @param scale scale of the object
	 * @param rotationOrder order of the multiplication of the rotation matricies
	 */
	Camera(
		const char* name = "Camera",
		Spatial* parent = nullptr,
		const bool visible = true,
		const uint32 tag = 0,
		const glm::vec3 position = { 0.0f, 0.0f, 0.0f },
		const glm::vec3 rotation = { 0.0f, 0.0f, 0.0f },
		const glm::vec3 scale = { 1.0f, 1.0f, 1.0f },
		const RotationOrder rotationOrder = RotationOrder::ROTATION_ZYX
	);

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
	std::vector<Material*> _materials;
	std::unique_ptr<GraphicsPipeline> _renderPipeline;

	float _fov = 45.0f, _near = 0.1f, _far = 20.0f, _depth = 1.0f;
	glm::vec4 _viewport = { 0.0f, 0.0f, 1.0f, 1.0f };
	Projection _projection;

	/**
	 * @brief record the command buffer
	 */
	void record_command_buffers() const override;

	friend class RenderSystem;
	friend class Material;
	friend class Mesh;
};

} // namespace lyra
