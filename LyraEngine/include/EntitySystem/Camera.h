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

#include <Common/Common.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <vector>
#include <Common/Array.h>

#include <glm/glm.hpp>
#include <Math/ComputePipeline.h>

#include <Common/UniquePointer.h>
#include <Common/ResourcePool.h>
#include <Common/Config.h>

#include <EntitySystem/Entity.h>

#include <Graphics/VulkanImpl/DescriptorSystem.h>
#include <Graphics/VulkanImpl/GPUBuffer.h>
#include <Graphics/GraphicsPipelineSystem.h>
#include <Graphics/Renderer.h>

namespace lyra {

/**
 * @brief wrapper arond a uniform buffer object
 */
class Camera : public Framebuffers, public ComponentBase {
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
	 * @param skybox skybox for the camera
	 * @param perspective check if perspective is true or not
	 */
	Camera(
		Skybox* skybox = nullptr,
		bool perspective = true
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
	void setPerspective(const float32& fov = 45.0f, const float32& near = 0.1f, const float32& far = 10.0f) noexcept;
	/**
	 * @brief set the projection of the camera in orthographic mode
	 *
	 * @param viewport viewport
	 * @param near near clipping plane
	 * @param far far clipping plane
	*/
	void setOrthographic(const glm::vec4& viewport = { 0.0f, 0.0f, 1.0f, 1.0f }, const float32& near = 0.1f, const float32& far = 20.0f) noexcept;
	/**
	 * @brief update function
	*/
	void update();

	/**
	 * @brief get the field of view of the camera
	 *
	 * @return float32
	*/
	NODISCARD float32 fov() const noexcept { return m_fov; }
	/**
	 * @brief get the near clipping plane of the camera
	 *
	 * @return float32
	*/
	NODISCARD float32 near() const noexcept { return m_near; }
	/**
	 * @brief get the far clipping plane of the camera
	 *
	 * @return float32
	*/
	NODISCARD float32 far() const noexcept { return m_far; }
	/**
	 * @brief get the viewport dimensions and positions
	 *
	 * @return glm::vec4
	*/
	NODISCARD glm::vec4 viewport() const noexcept { return m_viewport; }

private:
	std::vector<Material*> m_materials;
	Array<vulkan::GPUBuffer, config::maxFramesInFlight> m_buffers;
	Array<vulkan::DescriptorSystem::DescriptorSetResource, config::maxFramesInFlight> m_descriptorSets;
	ComputePipeline m_computePipeline;
	Skybox* m_skybox;

	float32 m_fov = 45.0f, m_near = 0.1f, m_far = 20.0f, m_depth = 1.0f;
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
