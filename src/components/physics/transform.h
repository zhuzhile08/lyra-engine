/*************************
 * @file transform.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief a wrapper around the Vulkan command buffers
 *
 * @date 2022-02-05
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <noud.h>
#include <glm.hpp>
#include <math/math.h>
#include <math.h>
#include <string>

namespace lyra {

// transformation component
class Transform : noud::Node {
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

	/**
	 * @brief construct a new transformation component
	 *
	 * @param position new position of the component
	 * @param rotation new rotation of the component
	 * @param scale new scale of the component
	 * @param visible visibility of the component
	 * @param rotationOrder order of the multiplication of the rotation matricies
	 * @param name name of the component
	 * @param parent parent transform object of the component
	*/
	Transform(
		const glm::vec3 position = { 0, 0, 0 }, 
		const glm::vec3 rotation = { 0, 0, 0 },
		const glm::vec3 scale = { 1, 1, 1 },
		const bool visible = true,
		const RotationOrder rotationOrder = RotationOrder::ROTATION_ZYX,
		const std::string name = "transform",
		Transform* const parent = nullptr
	);

	Transform(const Transform&) noexcept = delete;
	Transform operator=(const Transform&) const noexcept = delete;

	void translate(glm::vec3 velocity);
	void rotate(glm::vec3 rotation);
	void translate(glm::vec3 velocity, glm::vec3 point);
	void rotate(glm::vec3 rotation, glm::vec3 point);
	void show();
	void hide();
	void look_at();
	void update();
	void force_update();

	/**
	 * @brief set the position
	 *
	 * @param newPosition new position
	*/
	void set_position(glm::vec3 newPosition) noexcept;
	/**
	 * @brief set the rotation
	 *
	 * @param newRotation new rotation
	*/
	void set_rotation(glm::vec3 newRotation) noexcept;
	/**
	 * @brief set the scale
	 *
	 * @param newScale new scale
	*/
	void set_scale(glm::vec3 newScale) noexcept;

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
	 * @brief get the local position
	 *
	 * @return const glm::vec3
	*/
	[[nodiscard]] const glm::vec3 position_global() const noexcept {
		return _position + _parent->position_global();
	};
	/**
	 * @brief get the local rotation
	 *
	 * @return const glm::vec3
	*/
	[[nodiscard]] const glm::vec3 rotation_global() const noexcept { return _rotation + ((_parent == nullptr) ? glm::vec3(0.0f) : _parent->rotation_global()); };
	/**
	 * @brief get the local scale
	 *
	 * @return const glm::vec3
	*/
	[[nodiscard]] const glm::vec3 scale_global() const noexcept { return _scale + ((_parent == nullptr) ? glm::vec3(0.0f) : _parent->scale_global()); };
	/**
	 * @brief get the order of rotation
	 * 
	 * @return const lyra::Transform::RotationOrder
	*/
	[[nodiscard]] const RotationOrder rotationOrder() const noexcept { return _rotationOrder; }
	/**
	 * @brief get the parent transform object
	 *
	 * @return const lyra::Transform
	*/
	[[nodiscard]] const Transform* const parent() const noexcept { return _parent; }
	/**
	 * @brief get if the object is visible or not
	 *
	 * @return const bool
	*/
	[[nodiscard]] const bool visibility() const noexcept { return _visible; }

protected:
	glm::vec3 _position = { 0, 0, 0 }, _rotation = { 0, 0, 0 }, _scale = { 1, 1, 1 };
	RotationOrder _rotationOrder = RotationOrder::ROTATION_ZYX;
	glm::mat4 _transformMatrix = glm::mat4(1.0f);
	bool _visible = true;

	Transform* _parent = nullptr;
};

} // namespace lyra
