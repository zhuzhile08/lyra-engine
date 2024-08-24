#include <Components/Camera.h>

#include <Math/LyraMath.h>

#include <ETCS/Entity.h>
// #include <ECS/Cubemap.h>

#include <glm/gtc/matrix_transform.hpp>

namespace lyra {

void Camera::projectionPerspective(float32 aspect, float32 fov, float32 near, float32 far) noexcept {
	m_projection = Projection::perspective;
	m_fov = fov;
	m_near = near;
	m_far = far;
	m_aspect = aspect;
	m_projectionMatrix = glm::perspective(glm::radians(fov), m_aspect, m_near, m_far);
}

void Camera::projectionOrthographic(float32 near, float32 far) noexcept {
	m_projection = Projection::orthographic;
	m_near = near;
	m_far = far;
	// m_projectionMatrix = glm::ortho(m_viewport[0], m_viewport[1] + m_viewport[0], m_viewport[2], m_viewport[3] + m_viewport[2], m_near, m_far); implement orthographic rendering @todo
}

void Camera::update() {
	auto newAspect = renderer::drawWidth() * viewportSize.x / (float32) renderer::drawHeight() * viewportSize.y;

	if (m_projection == Projection::perspective && m_aspect != newAspect) {
		projectionPerspective(newAspect, m_fov, m_near, m_far);
	}
}

Camera::TransformData Camera::data(const glm::mat4& meshTransform) const noexcept {
	return TransformData {
		entity->component<Transform>().globalTransform(),
		m_projectionMatrix,
		meshTransform
	};
}

} // namespace lyra
