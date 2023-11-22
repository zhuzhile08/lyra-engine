#include <EntitySystem/Camera.h>

#include <Graphics/Renderer.h>

#include <Math/LyraMath.h>

#include <EntitySystem/Entity.h>
#include <EntitySystem/Script.h>
#include <EntitySystem/Transform.h>
// #include <EntitySystem/Cubemap.h>

#include <glm/gtc/matrix_transform.hpp>

namespace lyra {

void Camera::projectionPerspective(float32 fov, float32 near, float32 far) noexcept {
	m_projection = Projection::perspective;
	m_fov = fov;
	m_near = near;
	m_far = far;
	m_aspect = renderer::drawWidth() / (float32) renderer::drawHeight();
	m_projectionMatrix = glm::perspective(glm::radians(fov), m_aspect, m_near, m_far); 
	m_projectionMatrix[1][1] *= -1;
}

void Camera::projectionOrthographic(const glm::vec4& viewport, float32 near, float32 far) noexcept {
	m_projection = Projection::orthographic;
	m_viewport = viewport;
	m_near = near;
	m_far = far;
	m_projectionMatrix = glm::ortho(m_viewport[0], m_viewport[1] + m_viewport[0], m_viewport[2], m_viewport[3] + m_viewport[2], m_near, m_far);
	m_projectionMatrix[1][1] *= -1;
}

void Camera::update() {
	auto tmpAspect = renderer::drawWidth() / (float32) renderer::drawHeight();

	if (m_projection == Projection::perspective && m_aspect != tmpAspect) {
		m_aspect = tmpAspect;
		projectionPerspective(m_fov, m_near, m_far);
	}
}

const Camera::CameraData& Camera::data() noexcept { 
	return ( m_data = CameraData { m_entity->component<Transform>()->global_transform(), m_projectionMatrix } ); 
}

} // namespace lyra
