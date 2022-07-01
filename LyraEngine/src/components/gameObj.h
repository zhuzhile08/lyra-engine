/*************************
 * @file gameObj.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief An empty game object
 *
 * @date 2022-22-06
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#define GLM_FORCE_RADIANS

#include <math/math.h>

#include <noud.h>
#include <glm.hpp>
#include <cmath>
#include <gtc/matrix_transform.hpp>
#include <gtx/matrix_decompose.hpp>
#include <string>

namespace lyra {

class GameObject : noud::Node {
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
	 * @brief construct a game object
	 * 
	 * @param name name of the object
	 * @param parent parent node of the object
	 * @param visible visibility of the object
	 * @param tag optional tag of the object
	 * @param position position of the object
	 * @param rotation rotation of the object
	 * @param scale scale of the object
	 * @param rotationOrder order of the multiplication of the rotation matricies
	 */
	GameObject(
		const char* name = "Game Object", 
		GameObject* parent = nullptr,
		const bool visible = true,
		const uint32 tag = 0,
		const glm::vec3 position = { 0.0f, 0.0f, 0.0f },
		const glm::vec3 rotation = { 0.0f, 0.0f, 0.0f },
		const glm::vec3 scale = { 1.0f, 1.0f, 1.0f },
		const RotationOrder rotationOrder = RotationOrder::ROTATION_XYZ
	) noexcept : noud::Node(parent, name), _parent(parent), _visible(visible), _tag(tag), _position(position), _rotation(rotation), _scale(scale), _rotationOrder(rotationOrder) { init(); }

	/**
	 * @brief update function, which gets updated every frame
	 */
	virtual void update(void) { }
	/**
	 * @brief function called at initialization time
	 */
	virtual void init(void) { }

	/**
	 * @brief show the object
	 */
	void show() noexcept { _visible = true; }
	/**
	 * @brief hide the object
	 */
	void hide() noexcept { _visible = false; }

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
	void rotate(glm::vec3 rotation, Space space = Space::SPACE_LOCAL) { set_rotation(_rotation + rotation, space); }
	/**
	 * @brief look at a position
	 * 
	 * @param target target to look at
	 * @param up up vector
	 */
	void look_at(glm::vec3 target, glm::vec3 up = { 0.0f, 0.0f, 1.0f });
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
	void set_rotation(glm::vec3 newRotation, Space space = Space::SPACE_LOCAL) noexcept { 
		if (space == Space::SPACE_LOCAL) _rotation = newRotation;
		else _rotation = newRotation - rotation_global();

		_localTransformMatrix *= calculate_roation_mat(); 
	}

	/**
	 * @brief get the local position
	 *
	 * @return const glm::vec3
	*/
	[[nodiscard]] const glm::vec3 position_global() const noexcept {
		return _position + _parent->position_global();
	};
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
	 * @brief set the scale
	 *
	 * @param newScale new scale
	*/
	void set_scale(glm::vec3 newScale) noexcept { _scale = newScale; }
	/**
	 * @brief set the tag to a new one
	 *
	 * @param tag new tag
	 */
	void set_tag(const uint32 tag) noexcept { _tag = tag; }

	/**
	 * @brief get if the object is visible or not
	 *
	 * @return const uint32_t
	*/
	[[nodiscard]] const uint32 tag() const noexcept { return _tag; }
	/**
	 * @brief get if the object is visible or not
	 *
	 * @return const bool
	*/
	[[nodiscard]] const bool visibility() const noexcept { return _visible; }
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
	 * @return const lyra::Transform::RotationOrder
	*/
	[[nodiscard]] const RotationOrder rotationOrder() const noexcept { return _rotationOrder; }

protected:
	glm::vec3 _position = { 0.0f, 0.0f, 0.0f }, _rotation = { 0.0f, 0.0f, 0.0f }, _scale = { 1.0f, 1.0f, 1.0f };
	RotationOrder _rotationOrder = RotationOrder::ROTATION_XZY;

	glm::mat4 _localTransformMatrix = glm::mat4(1.0f);

	GameObject* _parent;
	
	bool _visible = true;

	uint32 _tag = 0; // you can go and define some tags yourself, just remember that 0 is always an undefined tag

private:
	/**
	 * @brief calculate the rotation matrix based on the parent rotation matrix and the current rotation
	 */
	[[nodiscard]] const glm::mat4 calculate_roation_mat() const;
};

} // namespace lyra