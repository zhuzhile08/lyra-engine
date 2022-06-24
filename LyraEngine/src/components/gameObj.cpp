#include <components/gameObj.h>

namespace lyra {

void GameObject::set_position(glm::vec3 newPosition, Space space) noexcept {
	if (space == Space::SPACE_LOCAL) {
		_position = newPosition;
	} else {
		glm::vec4 tempVec = glm::vec4{ newPosition[0], newPosition[1], newPosition[2], 1.0f } * _localRotationMatrix;
		_position = glm::vec3{ tempVec[0], tempVec[1], tempVec[2] };
	}
	_localTransformMatrix = glm::translate(glm::mat4(1.0f), _position);
}

void GameObject::look_at(glm::vec3 target, glm::vec3 up) {
	_localRotationMatrix = glm::lookAt(_position, target, up);

	// deconsturct the rotation matrix to get the new rotations
	_rotation.x = atan2(_localRotationMatrix[2][1], _localRotationMatrix[2][2]);
	_rotation.x = atan2(-_localRotationMatrix[0][2], sqrt(pow(_localRotationMatrix[0][0], 2) + pow(_localRotationMatrix[0][1], 2)));
	float c1 = cos(_rotation.x), s1 = sin(_rotation.x);
	_rotation.z = atan2(s1 * _localRotationMatrix[0][2] - c1 * _localRotationMatrix[0][1], c1 * _localRotationMatrix[1][1] - s1 * _localRotationMatrix[1][2]);
	// I got this from some paper, no idea if this works
}

const glm::mat4 GameObject::calculate_roation_mat() const {
	glm::mat4 mat1, mat2, mat3;

	// are you proud of me, Yandere Dev?
	if (_rotationOrder == RotationOrder::ROTATION_XYZ || _rotationOrder == RotationOrder::ROTATION_XZY) {
		mat1 = glm::rotate(glm::mat4{ 1 }, glm::radians(_rotation[0]), { 1.0f, 0.0f, 0.0f });
		if (_rotationOrder == RotationOrder::ROTATION_XYZ) {
			mat2 = glm::rotate(mat1, glm::radians(_rotation[1]), { 0.0f, 1.0f, 0.0f });
			mat3 = glm::rotate(mat2, glm::radians(_rotation[2]), { 0.0f, 0.0f, 1.0f });
		} else {
			mat2 = glm::rotate(mat1, glm::radians(_rotation[2]), { 0.0f, 0.0f, 1.0f });
			mat3 = glm::rotate(mat2, glm::radians(_rotation[1]), { 0.0f, 1.0f, 0.0f });
		}
	} else if (_rotationOrder == RotationOrder::ROTATION_YZX || _rotationOrder == RotationOrder::ROTATION_YXZ) {
		mat1 = glm::rotate(glm::mat4{ 1 }, glm::radians(_rotation[1]), { 0.0f, 1.0f, 0.0f });
		if (_rotationOrder == RotationOrder::ROTATION_YXZ) {
			mat2 = glm::rotate(mat1, glm::radians(_rotation[0]), { 1.0f, 0.0f, 0.0f });
			mat3 = glm::rotate(mat2, glm::radians(_rotation[2]), { 0.0f, 0.0f, 1.0f });
		}
		else {
			mat2 = glm::rotate(mat1, glm::radians(_rotation[2]), { 0.0f, 0.0f, 1.0f });
			mat3 = glm::rotate(mat2, glm::radians(_rotation[0]), { 1.0f, 0.0f, 0.0f });
		}
	} else {
		mat1 = glm::rotate(glm::mat4{ 1 }, glm::radians(_rotation[2]), { 0.0f, 0.0f, 1.0f });
		if (_rotationOrder == RotationOrder::ROTATION_ZXY) {
			mat2 = glm::rotate(mat1, glm::radians(_rotation[0]), { 1.0f, 0.0f, 0.0f });
			mat3 = glm::rotate(mat2, glm::radians(_rotation[1]), { 0.0f, 1.0f, 0.0f });
		}
		else {
			mat2 = glm::rotate(mat1, glm::radians(_rotation[1]), { 0.0f, 1.0f, 0.0f });
			mat3 = glm::rotate(mat2, glm::radians(_rotation[0]), { 1.0f, 0.0f, 0.0f });
		}
	} // yeah, the maximum amount of if statements this has to go through is 4. Every. Single. Frame.

	return mat3;
}

} // namespace lyra
