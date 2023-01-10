/*************************
 * @file camera.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief uniform buffer objects
 *
 * @date 2022-03-07
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <lyra.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <vector>
#include <core/array.h>

#include <glm.hpp>

#include <core/smart_pointer.h>
#include <core/settings.h>

#include <nodes/spatial.h>

#include <rendering/vulkan/descriptor.h>
#include <rendering/vulkan/GPU_buffer.h>
#include <rendering/renderer.h>

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
		alignas(16) const glm::mat4 model;
		alignas(16) const glm::mat4 proj;
	};
	
	/**
	 * @brief construct a camera node
	 *
	 * @param script script of the object
	 * @param skybox skybox for the camera
	 * @param perspective check if perspective is true or not
	 * @param name name of the object
	 * @param parent parent Node of the object
	 * @param visible visibility of the object
	 * @param tag optional tag of the object
	 * @param transform transform of the object
	 */
	Camera(
		Script* script,
		Skybox* skybox = nullptr,
		const bool& perspective = true,
		const char* name = "Camera",
		Spatial* parent = nullptr,
		const bool& visible = true,
		const uint32& tag = 0,
		const Transform& transform = Transform()
	);

#ifndef _WIN32
	~Camera() { }
#endif

	Camera operator=(const Camera&) const noexcept = delete;

	/**
	 * @brief recreate the camera
	 */
	void recreate();

	/**
	 * @brief set the projection of the camera in perspective mode
	 * 
	 * @param fov field of view
	 * @param near near clipping plane
	 * @param far far clipping plane
	*/
	void set_perspective(const float& fov = 45.0f, const float& near = 0.1f, const float& far = 10.0f) noexcept;
	/**
	 * @brief set the projection of the camera in orthographic mode
	 *
	 * @param viewport viewport
	 * @param near near clipping plane
	 * @param far far clipping plane
	*/
	void set_orthographic(const glm::vec4& viewport = { 0.0f, 0.0f, 1.0f, 1.0f }, const float& near = 0.1f, const float& far = 20.0f) noexcept;
	/**
	 * @brief get the field of view of the camera
	 *
	 * @return const float
	*/
	NODISCARD const float fov() const noexcept { return m_fov; }
	/**
	 * @brief get the near clipping plane of the camera
	 *
	 * @return const float
	*/
	NODISCARD const float near() const noexcept { return m_near; }
	/**
	 * @brief get the far clipping plane of the camera
	 *
	 * @return const float
	*/
	NODISCARD const float far() const noexcept { return m_far; }
	/**
	 * @brief get the viewport dimensions and positions
	 *
	 * @return const float
	*/
	NODISCARD const glm::vec4 viewport() const noexcept { return m_viewport; }
	/**
	 * @brief get the camera data buffers
	 * 
	 * @return const std::vector<vulkan::GPUBuffer>&
	 */
	NODISCARD const std::vector<vulkan::GPUBuffer>& buffers() const noexcept { return m_buffers; }

private:
	std::vector<Material*> m_materials;
	std::vector<vulkan::GPUBuffer> m_buffers;
	std::vector<vulkan::Descriptor> m_descriptors;
	SmartPointer<GraphicsPipeline> m_renderPipeline;  
	Skybox* m_skybox;

	float m_fov = 45.0f, m_near = 0.1f, m_far = 20.0f, m_depth = 1.0f;
	glm::vec4 m_viewport = { 0.0f, 0.0f, 1.0f, 1.0f };
	Projection m_projection;
	glm::mat4 m_projection_matrix;

	/**
	 * @brief draw function
	*/
	void draw();
	/**
	 * @brief record the command buffer
	 */
	void record_command_buffers() override;

	friend class CubemapBase;
	friend class Skybox;
	friend class RenderSystem;
	friend class Material;
	friend class Mesh;
};

} // namespace lyra
