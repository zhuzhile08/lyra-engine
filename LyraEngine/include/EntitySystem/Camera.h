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

#include <Graphics/Renderer.h>
#include <Graphics/VulkanRenderSystem.h>

#include <glm/glm.hpp>

#include <vector>

namespace lyra {

class Camera : public ComponentBase {
public:
	enum class Projection {
		perspective,
		orthographic
	};

	enum class Clear {
		skybox,
		color,
		none
	};

	struct CameraData {
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 proj;
	};

	Camera(
		float32 fov = 45.0f, 
		float32 near = 0.1f, 
		float32 far = 10.0f
	);
	Camera(
		const glm::vec4& viewport, 
		float32 near = 0.1f, 
		float32 far = 20.0f
	);

	void projectionPerspective(float32 fov = 45.0f, float32 near = 0.1f, float32 far = 10.0f) noexcept;
	void projectionOrthographic(const glm::vec4& viewport = { 0.0f, 0.0f, 1.0f, 1.0f }, float32 near = 0.1f, float32 far = 20.0f) noexcept;

	void update();

	NODISCARD float32 fov() const noexcept { return m_fov; }
	NODISCARD float32 near() const noexcept { return m_near; }
	NODISCARD float32 far() const noexcept { return m_far; }
	NODISCARD float32 aspect() const noexcept { return m_aspect; }
	NODISCARD glm::vec4 viewport() const noexcept { return m_viewport; }

private:
	Array<vulkan::GPUBuffer, config::maxFramesInFlight> m_buffers;
	vulkan::DescriptorSets m_descriptorSets;

	uint32 currentIndex;

	Projection m_projection;

	float32 m_fov = 45.0f, m_near = 0.1f, m_far = 20.0f, m_depth = 1.0f, m_aspect = config::windowWidth / (float32) config::windowHeight;

	glm::vec4 m_viewport = { 0.0f, 0.0f, 1.0f, 1.0f };
	glm::mat4 m_projection_matrix = glm::mat4(1.0f);

	friend class renderSystem::Renderer;
};

} // namespace lyra
