/*************************
 * @file Camera.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief uniform buffer objects
 *
 * @date 2022-03-07
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <Lyra/Lyra.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <vector>
#include <Common/Array.h>

#include <glm/glm.hpp>

#include <Common/SmartPointer.h>
#include <Common/ResourcePool.h>
#include <Common/Settings.h>

#include <EntitySystem/Entity.h>

#include <Graphics/VulkanImpl/DescriptorSystem.h>
#include <Graphics/VulkanImpl/GPUBuffer.h>
#include <Graphics/GraphicsPipeline.h>
#include <Graphics/Renderer.h>

namespace lyra {

/**
 * @brief wrapper arond a uniform buffer object
 */
class Camera : public Renderer, public ComponentBase {
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
	
	Camera() = default;
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
		const bool& perspective = true
	);
	DEFINE_DEFAULT_MOVE(Camera)

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
	 * @brief update function
	*/
	void update() override;

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

private:
	std::vector<Material*> m_materials;
	Array<vulkan::GPUBuffer, Settings::RenderConfig::maxFramesInFlight> m_buffers;
	Array<vulkan::DescriptorSystem::DescriptorSetResource, Settings::RenderConfig::maxFramesInFlight> m_descriptorSets;
	GraphicsPipeline m_renderPipeline;  
	Skybox* m_skybox;

	float m_fov = 45.0f, m_near = 0.1f, m_far = 20.0f, m_depth = 1.0f;
	glm::vec4 m_viewport = { 0.0f, 0.0f, 1.0f, 1.0f };
	Projection m_projection;
	glm::mat4 m_projection_matrix;

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
