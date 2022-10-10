#include <nodes/spatial.h>

#include <gtc/matrix_transform.hpp>
#include <gtx/matrix_decompose.hpp>
#include <cmath>

namespace lyra {

void Spatial::set_position(glm::vec3 newPosition, Space space) noexcept {
	if (space == Space::SPACE_LOCAL) {
		m_position = newPosition;
	}
	else {
		glm::vec4 tempVec = glm::vec4{ newPosition[0], newPosition[1], newPosition[2], 1.0f } *m_localTransformMatrix;
		m_position = glm::vec3{ tempVec[0], tempVec[1], tempVec[2] };
	}
}

void Spatial::set_rotation(glm::vec3 newRotation, Space space) noexcept {
	if (space == Space::SPACE_LOCAL) m_rotation = newRotation;
	else m_rotation = newRotation - rotation_global();
}

void Spatial::look_at(glm::vec3 target, glm::vec3 up) {
	m_localTransformMatrix = glm::lookAt(m_position, target, up);

	// deconsturct the rotation matrix to get the new rotations
	m_rotation.x = glm::degrees(atan2(m_localTransformMatrix[2][1], m_localTransformMatrix[2][2]));
	m_rotation.y = glm::degrees(atan2(-m_localTransformMatrix[2][0], sqrt(pow(m_localTransformMatrix[0][0], 2) + pow(m_localTransformMatrix[1][0], 2))));
	m_rotation.z = glm::degrees(atan2(m_localTransformMatrix[1][0], m_localTransformMatrix[0][0]));
}

void Spatial::calculate_transform_mat() {
	glm::mat4 mat1, mat2;

	// are you proud of me, Yandere Dev?
	if (m_rotationOrder == RotationOrder::ROTATION_XYZ || m_rotationOrder == RotationOrder::ROTATION_XZY) {
		mat1 = glm::rotate(glm::mat4{ 1 }, glm::radians(m_rotation[0]), { 1.0f, 0.0f, 0.0f });
		if (m_rotationOrder == RotationOrder::ROTATION_XYZ) {
			mat2 = glm::rotate(mat1, glm::radians(m_rotation[1]), { 0.0f, 1.0f, 0.0f });
			m_localTransformMatrix *= glm::rotate(mat2, glm::radians(m_rotation[2]), { 0.0f, 0.0f, 1.0f });
		}
		else {
			mat2 = glm::rotate(mat1, glm::radians(m_rotation[2]), { 0.0f, 0.0f, 1.0f });
			m_localTransformMatrix *= glm::rotate(mat2, glm::radians(m_rotation[1]), { 0.0f, 1.0f, 0.0f });
		}
	}
	else if (m_rotationOrder == RotationOrder::ROTATION_YZX || m_rotationOrder == RotationOrder::ROTATION_YXZ) {
		mat1 = glm::rotate(glm::mat4{ 1 }, glm::radians(m_rotation[1]), { 0.0f, 1.0f, 0.0f });
		if (m_rotationOrder == RotationOrder::ROTATION_YXZ) {
			mat2 = glm::rotate(mat1, glm::radians(m_rotation[0]), { 1.0f, 0.0f, 0.0f });
			m_localTransformMatrix *= glm::rotate(mat2, glm::radians(m_rotation[2]), { 0.0f, 0.0f, 1.0f });
		}
		else {
			mat2 = glm::rotate(mat1, glm::radians(m_rotation[2]), { 0.0f, 0.0f, 1.0f });
			m_localTransformMatrix *= glm::rotate(mat2, glm::radians(m_rotation[0]), { 1.0f, 0.0f, 0.0f });
		}
	}
	else {
		mat1 = glm::rotate(glm::mat4{ 1 }, glm::radians(m_rotation[2]), { 0.0f, 0.0f, 1.0f });
		if (m_rotationOrder == RotationOrder::ROTATION_ZXY) {
			mat2 = glm::rotate(mat1, glm::radians(m_rotation[0]), { 1.0f, 0.0f, 0.0f });
			m_localTransformMatrix *= glm::rotate(mat2, glm::radians(m_rotation[1]), { 0.0f, 1.0f, 0.0f });
		}
		else {
			mat2 = glm::rotate(mat1, glm::radians(m_rotation[1]), { 0.0f, 1.0f, 0.0f });
			m_localTransformMatrix *= glm::rotate(mat2, glm::radians(m_rotation[0]), { 1.0f, 0.0f, 0.0f });
		}
	} // yeah, the maximum amount of if statements this has to go through is 4. Every. Single. Frame.

	// calculate the translation
	m_localTransformMatrix *= glm::translate(glm::mat4(1.0f), m_position);
}

} // namespace lyra
