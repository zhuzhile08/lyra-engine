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
	glm::vec3 temp_translation(m_translation);
	if(m_dirty) {
		m_localTransformMatrix = glm::lookAt(glm::vec3(m_localTransformMatrix[3]), target, up);
		decompose_transform_matrix(m_localTransformMatrix, m_translation, m_rotation, m_scale);
		m_translation = temp_translation;
		m_dirty = false;
	} else {
		m_localTransformMatrix = glm::lookAt(glm::vec3(m_localTransformMatrix[3]), target, up);
		decompose_transform_matrix(m_localTransformMatrix, temp_translation, m_rotation, m_scale);
	}
}

void Transform::translate_and_look_at(const glm::vec3& translate, const glm::vec3& target, const glm::vec3& up) {
	this->translate(translate);
	glm::vec3 temp_translation(translate);
	m_localTransformMatrix = glm::lookAt(glm::vec3(m_localTransformMatrix[3]), target, up);
	decompose_transform_matrix(m_localTransformMatrix, m_translation, m_rotation, m_scale);
	m_translation = temp_translation;
	m_dirty = false;
}

void Transform::set_translation(const glm::vec3& translation) {
	m_localTransformMatrix[3] = {0.0f, 0.0f, 0.0f, 1.0f};
	m_localTransformMatrix = glm::translate(m_localTransformMatrix, translation);
	m_translation = translation;
	m_dirty = false;
}

void Transform::set_rotation(const glm::vec3& rotation) {
	if (m_dirty) {
		decompose_transform_matrix(m_localTransformMatrix, m_translation, m_rotation, m_scale);
		m_dirty = false;
	}
	(*this) = Transform(m_translation, rotation, m_scale);
	m_rotation = rotation;
}
/**
 * @brief set the scale of the matrix
 * 
 * @param scale new scale
 */
void Transform::set_scale(const glm::vec3& scale) {
	if (m_dirty) {
		decompose_transform_matrix(m_localTransformMatrix, m_translation, m_rotation, m_scale);
		m_dirty = false;
	}
	(*this) = Transform(m_translation, m_rotation, scale);
	m_scale = scale;
}

constexpr glm::vec3 Transform::translation() noexcept { 
	// check if the vectors are m_dirty, if so, recalculate them
	if (m_dirty) {
		decompose_transform_matrix(m_localTransformMatrix, m_translation, m_rotation, m_scale);
		m_dirty = false;
	}
	return m_translation; 
}

constexpr glm::vec3 Transform::rotation() noexcept { 
	// check if the vectors are m_dirty, if so, recalculate them
	if (m_dirty) {
		decompose_transform_matrix(m_localTransformMatrix, m_translation, m_rotation, m_scale);
		m_dirty = false;
	}
	return m_rotation;
}

constexpr glm::vec3 Transform::scale() noexcept { 
	// check if the vectors are m_dirty, if so, recalculate them
	if (m_dirty) {
		decompose_transform_matrix(m_localTransformMatrix, m_translation, m_rotation, m_scale);
		m_dirty = false;
	}
	return m_scale; 
}

} // namespace lyra