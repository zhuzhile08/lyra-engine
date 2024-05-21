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
#include <Common/BasicRenderer.h>
#include <Common/Config.h>

#include <ECS/Entity.h>
#include <ECS/ECS.h>
#include <ECS/Components/Transform.h>

#include <Graphics/Renderer.h>
#include <Graphics/VulkanRenderSystem.h>

#include <glm/glm.hpp>

#include <Common/Vector.h>

namespace lyra {

class Camera : public BasicComponent {
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

	struct TransformData {
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 proj;
		alignas(16) glm::mat4 mesh;
	};

	Camera(float32 fov = 45.0f, float32 near = 0.1f, float32 far = 100.0f) {
		projectionPerspective(config::windowWidth / static_cast<float32>(config::windowHeight), fov, near, far);
	}
	Camera(float32 near, float32 far) {
		projectionOrthographic(near, far);
	}

	void projectionPerspective(float32 aspect, float32 fov = 45.0f, float32 near = 0.1f, float32 far = 100.0f) noexcept;
	void projectionOrthographic(float32 near = 0.1f, float32 far = 20.0f) noexcept;

	void update();

	NODISCARD constexpr float32 fov() const noexcept { return m_fov; }
	NODISCARD constexpr float32 near() const noexcept { return m_near; }
	NODISCARD constexpr float32 far() const noexcept { return m_far; }
	NODISCARD constexpr float32 aspect() const noexcept { return m_aspect; }
	NODISCARD TransformData data(const glm::mat4& meshTransform) const noexcept;

	glm::vec2 viewportSize = { 1.0f, 1.0f };
	glm::vec2 viewportPosition = { 0.0f, 0.0f };

private:
	Projection m_projection;

	float32 m_fov = 45.0f, m_near = 0.1f, m_far = 20.0f, m_aspect = config::windowWidth / (float32) config::windowHeight;
	glm::mat4 m_projectionMatrix = glm::mat4(1.0f);
};

} // namespace lyra
