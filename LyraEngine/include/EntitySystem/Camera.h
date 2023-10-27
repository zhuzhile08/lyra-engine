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

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <Common/Common.h>
#include <Common/Array.h>
#include <Common/UniquePointer.h>
#include <Common/RenderSystem.h>
#include <Common/Config.h>

#include <EntitySystem/Entity.h>

#include <Graphics/VulkanRenderSystem.h>

#include <glm/glm.hpp>

#include <vector>

namespace lyra {

class Camera : public ComponentBase, public QueuedRenderSystem, public RenderObject {
public:
	enum Projection {
		PROJECTION_PERSPECTIVE,
		PROJECTION_ORTHOGRAPHIC
	};

	enum Clear {
		CLEAR_SKYBOX,
		CLEAR_COLOR,
		CLEAR_NONE
	};

	struct CameraData {
		alignas(16) const glm::mat4 model;
		alignas(16) const glm::mat4 proj;
	};
	Camera(
		// Skybox* skybox = nullptr,
		bool perspective = true
	);
	DEFINE_DEFAULT_MOVE(Camera)

	void recreate();

	void setPerspective(const float32& fov = 45.0f, const float32& near = 0.1f, const float32& far = 10.0f) noexcept;
	void setOrthographic(const glm::vec4& viewport = { 0.0f, 0.0f, 1.0f, 1.0f }, const float32& near = 0.1f, const float32& far = 20.0f) noexcept;

	void update();

	NODISCARD float32 fov() const noexcept { return m_fov; }
	NODISCARD float32 near() const noexcept { return m_near; }
	NODISCARD float32 far() const noexcept { return m_far; }
	NODISCARD glm::vec4 viewport() const noexcept { return m_viewport; }

private:
	std::vector<Material*> m_materials;
	Array<vulkan::GPUBuffer, config::maxFramesInFlight> m_buffers;
	Array<vulkan::DescriptorSystem::DescriptorSetResource, config::maxFramesInFlight> m_descriptorSets;
	// ComputePipeline m_computePipeline;
	// Skybox* m_skybox;

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
