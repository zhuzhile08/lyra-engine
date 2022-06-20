#include <components/physics/transform.h>

namespace lyra {

Transform::Transform(
	const glm::vec3 position,
	const glm::vec3 rotation,
	const glm::vec3 scale,
	const bool visible,
	const RotationOrder rotationOrder,
	const std::string name,
	Transform* const parent
) : _position(position), _rotation(rotation), _scale(scale), _visible(visible), _rotationOrder(rotationOrder), _parent(parent), noud::Node(parent, name) { }

void Transform::set_position(glm::vec3 position) noexcept {
	_position = position;
}

void Transform::set_rotation(glm::vec3 rotation) noexcept {
	_rotation = rotation;
}

void Transform::set_scale(glm::vec3 scale) noexcept {
	_scale = scale;
}

} // namespace lyra
