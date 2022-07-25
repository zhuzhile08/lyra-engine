/*************************
 * @file   transform.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief  transformation component
 * 
 * @date   2022-24-7
 * 
 * @copyright Copyright (c) 2022
*************************/

#pragma once

#define GLM_FORCE_RADIANS

#include <core/core.h>
#include <components/node.h>

#include <glm.hpp>

namespace lyra {

class Spatial : public Node<Spatial> {
public:
	// order of transformation the object should use
	enum class RotationOrder { // since "normal" people think with euler angles and not that black magic quaternion stuff, this should work fine
		ROTATION_XYZ,
		ROTATION_XZY,
		ROTATION_YXZ,
		ROTATION_YZX,
		ROTATION_ZXY,
		ROTATION_ZYX
	};

	// space to transform/rotate/scale the object
	enum class Space {
		SPACE_LOCAL,
		SPACE_GLOBAL
	};

	/**
	 * @brief construct a transform component
	 *
	 * @param name name of the object
	 * @param parent parent Node of the object
	 * @param visible visibility of the object
	 * @param tag optional tag of the object
	 * @param position position of the object
	 * @param rotation rotation of the object
	 * @param scale scale of the object
	 * @param rotationOrder order of the multiplication of the rotation matricies
	 */
	Spatial(
		const char* name = "Game Object",
		Spatial* parent = nullptr,
		const bool visible = true,
		const uint32 tag = 0,
		const glm::vec3 position = { 0.0f, 0.0f, 0.0f },
		const glm::vec3 rotation = { 0.0f, 0.0f, 0.0f },
		const glm::vec3 scale = { 1.0f, 1.0f, 1.0f },
		const RotationOrder rotationOrder = RotationOrder::ROTATION_ZYX
	) noexcept : Node(name, parent, visible, tag), _position(position), _rotation(rotation), _scale(scale), _rotationOrder(rotationOrder) { }

	/**
	 * @brief update function, which gets updated every frame
	 */
	void update() override;

	/**
	 * @brief move the object
	 *
	 * @param velocity velocity to move the object
	 * @param space space to translate the object in
	 */
	void translate(glm::vec3 velocity, Space space = Space::SPACE_LOCAL) { set_position(_position + velocity, space); }
	/**
	 * @brief rotate the game object
	 *
	 * @param rotation rotation in degrees
	 * @param space space to rotate the object in
	 */
	void rotate(glm::vec3 rotation, Space space = Space::SPACE_LOCAL) {
		set_rotation(_rotation + rotation, space);
	}
	/**
	 * @brief look at a position
	 *
	 * @param target target to look at
	 * @param up up vector
	 */
	void look_at(glm::vec3 target, glm::vec3 up = { 0.0f, 0.0f, 1.0f });
	
	/**
	 * @brief get the local position
	 *
	 * @return const glm::vec3
	*/
	[[nodiscard]] const glm::vec3 position_global() const noexcept { return _position + _parent->position_global(); };
	/**
	 * @brief get the global rotation
	 *
	 * @return const glm::vec3
	*/
	[[nodiscard]] const glm::vec3 rotation_global() const noexcept { return _rotation + ((_parent == nullptr) ? glm::vec3(0.0f) : _parent->rotation_global()); };
	/**
	 * @brief get the global scale
	 *
	 * @return const glm::vec3
	*/
	[[nodiscard]] const glm::vec3 scale_global() const noexcept { return _scale + ((_parent == nullptr) ? glm::vec3(0.0f) : _parent->scale_global()); };
	/**
	 * @brief convert the local matrix to a global one
	 *
	 * @return const glm::vec4
	*/
	[[nodiscard]] const glm::mat4 mat_to_global() const noexcept { return _localTransformMatrix + ((_parent == nullptr) ? glm::mat4(0.0f) : _parent->mat_to_global()); };

	/**
	 * @brief set the position
	 *
	 * @param newPosition new position
	 * @param space space to move the object in
	*/
	void set_position(glm::vec3 newPosition, Space space = Space::SPACE_LOCAL) noexcept;
	/**
	 * @brief set the rotation
	 *
	 * @param newRotation new rotation in degrees
	 * @param space space to move the object in
	*/
	void set_rotation(glm::vec3 newRotation, Space space = Space::SPACE_LOCAL) noexcept;
	/**
	 * @brief set the scale
	 *
	 * @param newScale new scale
	*/
	void set_scale(glm::vec3 newScale) noexcept { _scale = newScale; }

	/**
	 * @brief get the local position
	 *
	 * @return const glm::vec3
	*/
	[[nodiscard]] const glm::vec3 position_local() const noexcept { return _position; };
	/**
	 * @brief get the local rotation
	 *
	 * @return const glm::vec3
	*/
	[[nodiscard]] const glm::vec3 rotation_local() const noexcept { return _rotation; };
	/**
	 * @brief get the local scale
	 *
	 * @return const glm::vec3
	*/
	[[nodiscard]] const glm::vec3 scale_local() const noexcept { return _scale; };
	/**
	 * @brief get the order of rotation
	 *
	 * @return const lyra::Spatial::RotationOrder
	*/
	[[nodiscard]] const RotationOrder rotationOrder() const noexcept { return _rotationOrder; }

protected:
	glm::vec3 _position = { 0.0f, 0.0f, 0.0f }, _rotation = { 0.0f, 0.0f, 0.0f }, _scale = { 1.0f, 1.0f, 1.0f };
	RotationOrder _rotationOrder = RotationOrder::ROTATION_ZYX;
	glm::mat4 _localTransformMatrix = glm::mat4(1.0f);

private:
	/**
	 * @brief calculate the transformation matrix based on the parent rotation matrix and the current rotation
	 */
	void calculate_transform_mat();
};

} // namespace lyra
