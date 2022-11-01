#include <math/transform.h>

namespace lyra {

void Transform::rotate(const glm::vec3& rotation, const RotationOrderXYZ&) {
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, rotation.x, {1.0f, 0.0f, 0.0f});
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, rotation.y, {0.0f, 1.0f, 0.0f});
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, rotation.z, {0.0f, 0.0f, 1.0f});
	m_dirty = true;
}

void Transform::rotate(const glm::vec3& rotation, const RotationOrderXZY&) {
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, rotation.x, {1.0f, 0.0f, 0.0f});
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, rotation.z, {0.0f, 0.0f, 1.0f});
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, rotation.y, {0.0f, 1.0f, 0.0f});
	m_dirty = true;
}

void Transform::rotate(const glm::vec3& rotation, const RotationOrderYXZ&) {
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, rotation.y, {0.0f, 1.0f, 0.0f});
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, rotation.x, {1.0f, 0.0f, 0.0f});
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, rotation.z, {0.0f, 0.0f, 1.0f});
	m_dirty = true;
}

void Transform::rotate(const glm::vec3& rotation, const RotationOrderYZX&) {
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, rotation.y, {0.0f, 1.0f, 0.0f});
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, rotation.z, {0.0f, 0.0f, 1.0f});
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, rotation.x, {1.0f, 0.0f, 0.0f});
	m_dirty = true;
}

void Transform::rotate(const glm::vec3& rotation, const RotationOrderZXY&) {
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, rotation.z, {0.0f, 0.0f, 1.0f});
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, rotation.x, {1.0f, 0.0f, 0.0f});
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, rotation.y, {0.0f, 1.0f, 0.0f});
	m_dirty = true;
}

void Transform::rotate(const glm::vec3& rotation, const RotationOrderZYX&) {
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, rotation.z, {0.0f, 0.0f, 1.0f});
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, rotation.y, {0.0f, 1.0f, 0.0f});
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, rotation.x, {1.0f, 0.0f, 0.0f});
	m_dirty = true;
}

void Transform::rotate(const float& x, const float& y, const float& z, const RotationOrderXYZ&) {
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, x, {1.0f, 0.0f, 0.0f});
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, y, {0.0f, 1.0f, 0.0f});
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, z, {0.0f, 0.0f, 1.0f});
	m_dirty = true;
}

void Transform::rotate(const float& x, const float& y, const float& z, const RotationOrderXZY&) {
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, x, {1.0f, 0.0f, 0.0f});
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, z, {0.0f, 0.0f, 1.0f});
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, y, {0.0f, 1.0f, 0.0f});
	m_dirty = true;
}

void Transform::rotate(const float& x, const float& y, const float& z, const RotationOrderYXZ&) {
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, y, {0.0f, 1.0f, 0.0f});
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, x, {1.0f, 0.0f, 0.0f});
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, z, {0.0f, 0.0f, 1.0f});
	m_dirty = true;
}

void Transform::rotate(const float& x, const float& y, const float& z, const RotationOrderYZX&) {
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, y, {0.0f, 1.0f, 0.0f});
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, z, {0.0f, 0.0f, 1.0f});
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, x, {1.0f, 0.0f, 0.0f});
	m_dirty = true;
}

void Transform::rotate(const float& x, const float& y, const float& z, const RotationOrderZXY&) {
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, z, {0.0f, 0.0f, 1.0f});
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, x, {1.0f, 0.0f, 0.0f});
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, y, {0.0f, 1.0f, 0.0f});
	m_dirty = true;
}

void Transform::rotate(const float& x, const float& y, const float& z, const RotationOrderZYX&) {
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, z, {0.0f, 0.0f, 1.0f});
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, y, {0.0f, 1.0f, 0.0f});
	m_localTransformMatrix = glm::rotate(m_localTransformMatrix, x, {1.0f, 0.0f, 0.0f});
	m_dirty = true;
}

void Transform::look_at(const glm::vec3& target, const glm::vec3& up) {
	if (m_dirty) decompose_transform_matrix(m_localTransformMatrix, m_translation, m_scale, m_rotation);
	m_localTransformMatrix = glm::lookAt(m_translation, target, up);
	m_dirty = true;
}

void Transform::translate_and_look_at(const glm::vec3& translate, const glm::vec3& target, const glm::vec3& up) {
	this->translate(translate);
	decompose_transform_matrix(m_localTransformMatrix, m_translation, m_rotation, m_scale);
	m_localTransformMatrix = glm::lookAt(m_translation, target, up);
	m_dirty = true;
}

void Transform::set_translation(const glm::vec3& translation) {
	m_localTransformMatrix[3] = {0.0f, 0.0f, 0.0f, 1.0f};
	glm::translate(m_localTransformMatrix, translation);
	m_dirty = true;
}

void Transform::set_rotation(const glm::vec3& rotation) {
	if (m_dirty) decompose_transform_matrix(m_localTransformMatrix, m_translation, m_rotation, m_scale);
	(*this) = Transform(m_translation, rotation, m_scale);
	m_rotation = rotation;
	m_dirty = false;
}
/**
 * @brief set the scale of the matrix
 * 
 * @param scale new scale
 */
void Transform::set_scale(const glm::vec3& scale) {
	if (m_dirty) decompose_transform_matrix(m_localTransformMatrix, m_translation, m_rotation, m_scale);
	(*this) = Transform(m_translation, m_rotation, scale);
	m_scale = scale;
	m_dirty = false;
}

} // namespace lyra