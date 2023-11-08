#include <EntitySystem/Camera.h>

#include <Graphics/Renderer.h>

#include <Math/LyraMath.h>

#include <EntitySystem/Entity.h>
#include <EntitySystem/Script.h>
#include <EntitySystem/Transform.h>
// #include <EntitySystem/Cubemap.h>

#include <glm/gtc/matrix_transform.hpp>

namespace lyra {

Camera::Camera(float32 fov, float32 near, float32 far) : m_descriptorSets(0) {
	for (auto& buffer : m_buffers) { 
		buffer = vulkan::GPUBuffer(sizeof(CameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
		m_descriptorSets.addWrites({
			{ { buffer.getDescriptorBufferInfo() }, 0, lyra::vulkan::DescriptorSets::Type::uniformBuffer }
		});
	}

	projectionPerspective(fov, near, far);
}

Camera::Camera(const glm::vec4& viewport, float32 near, float32 far) : m_descriptorSets(0) {
	std::vector<VkDescriptorBufferInfo> bufferInfos(config::maxFramesInFlight);

	for (auto& buffer : m_buffers) { 
		buffer = vulkan::GPUBuffer(sizeof(CameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
		m_descriptorSets.addWrites({
			{ { buffer.getDescriptorBufferInfo() }, 0, lyra::vulkan::DescriptorSets::Type::uniformBuffer }
		});
	}

	projectionOrthographic(viewport, near, far);
}

void Camera::projectionPerspective(float32 fov, float32 near, float32 far) noexcept {
	m_projection = Projection::perspective;
	m_fov = fov;
	m_near = near;
	m_far = far;
	m_aspect = renderSystem::drawWidth() / (float32) renderSystem::drawHeight();
	m_projection_matrix = glm::perspective(glm::radians(fov), m_aspect, m_near, m_far); 
	m_projection_matrix[1][1] *= -1;
}

void Camera::projectionOrthographic(const glm::vec4& viewport, float32 near, float32 far) noexcept {
	m_projection = Projection::orthographic;
	m_viewport = viewport;
	m_near = near;
	m_far = far;
	m_projection_matrix = glm::ortho(m_viewport[0], m_viewport[1] + m_viewport[0], m_viewport[2], m_viewport[3] + m_viewport[2], m_near, m_far);
	m_projection_matrix[1][1] *= -1;
}

void Camera::update() {
	auto tmpAspect = renderSystem::drawWidth() / (float32) renderSystem::drawHeight();

	if (m_projection == Projection::perspective && m_aspect != tmpAspect) {
		m_aspect = tmpAspect;
		projectionPerspective(m_fov, m_near, m_far);
	}

	// check wich projection model the camera uses and calculate the projection data
	CameraData data { m_entity->component<Transform>()->global_transform(), m_projection_matrix };
	// copy the data into the current buffer
	m_buffers[renderSystem::currentFrameIndex()].copyData(&data);
}

} // namespace lyra
