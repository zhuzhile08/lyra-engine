/*************************
 * @file transform.h
 * @author zhuzhile08 (zhuzhile08@gmail.com)
 * 
 * @brief a class that handels transformations
 * 
 * @date 2022-10-23
 * 
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <lyra.h>

#include <glm.hpp>
#include <gtx/matrix_decompose.hpp>

#include <math/math.h>

namespace lyra {

class Transform {
public:
	/**
	 * @brief construct a transform component
	 * 
	 * @param translation translation of the object
	 * @param rotation rotation of the object
	 * @param scale scale of the object 
	 */
#ifdef _WIN32
	Transform(
#else
	constexpr Transform(
#endif
		const glm::vec3& translation = glm::vec3(0.0f, 0.0f, 0.0f), 
		const glm::vec3& rotation = glm::vec3(0.0f, 0.0f, 0.0f), 
		const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f)
	) : m_localTransformMatrix(1.0f), m_translation(translation), m_rotation(rotation), m_scale(scale) {
		rotate(rotation);
		translate(translation);
		this->scale(scale);
	}
	/**
	 * @brief construct a transform component
	 * 
	 * @param mat transformation matrix
	 * @param rotationOrder order to calculate the rotations in
	 */
#ifdef _WIN32
	Transform(
#else
	constexpr Transform(
#endif
		const glm::mat4& mat
	) : m_localTransformMatrix(mat), m_dirty(false) { 
		decompose_transform_matrix(m_localTransformMatrix, m_translation, m_rotation, m_scale); 
	}

	/**
	 * @brief copy the matrix of a transform and recalculate the vectors for the destination (this) transform
	 * 
	 * @param transform the other transform
	 * @return constexpr Transform& 
	 */
#ifdef _WIN32
	Transform& 
#else
	constexpr Transform&
#endif
	operator=(const Transform& transform) {
		m_localTransformMatrix = transform.m_localTransformMatrix;
		decompose_transform_matrix(m_localTransformMatrix, m_translation, m_rotation, m_scale);
		m_dirty = false;
		return *this;
	}

	/**
	 * @brief translate the object
	 * 
	 * @param transform value to translate by
	 */
	void translate(const glm::vec3& translate) {
		m_localTransformMatrix = glm::translate(m_localTransformMatrix, translate);
		m_dirty = true;
	}
	/**
	 * @brief translate the object
	 * 
	 * @param x x value
	 * @param y y value
	 * @param z z value
	 */
	void translate(const float& x, const float& y, const float& z) {
		m_localTransformMatrix = glm::translate(m_localTransformMatrix, {x, y, z});
		m_dirty = true;
	}
	/**
	 * @brief translate the x axis
	 * 
	 * @param x x translation
	 */
	void translate_x(const float& x) {
		m_localTransformMatrix = glm::translate(m_localTransformMatrix, {x, 0.0f, 0.0f});
		m_dirty = true;
	}
	/**
	 * @brief translate the y axis
	 * 
	 * @param y y translation
	 */
	void translate_y(const float& y) {
		m_localTransformMatrix = glm::translate(m_localTransformMatrix, {0.0f, y, 0.0f});
		m_dirty = true;
	}
	/**
	 * @brief translate the z axis
	 * 
	 * @param z z rotation
	 */
	void translate_z(const float& z) {
		m_localTransformMatrix = glm::translate(m_localTransformMatrix, {0.0f, 0.0f, z});
		m_dirty = true;
	}

	struct RotationOrderXYZ {};
	struct RotationOrderXZY {};
	struct RotationOrderYXZ {};
	struct RotationOrderYZX {};
	struct RotationOrderZXY {};
	struct RotationOrderZYX {};

	/**
	 * @brief rotate by a value
	 * 
	 * @param rotation value to rotate by
	 */
	void rotate(const glm::vec3& rotation, const RotationOrderXYZ&);
	/**
	 * @brief rotate by a value
	 * 
	 * @param rotation value to rotate by
	 */
	void rotate(const glm::vec3& rotation, const RotationOrderXZY&);
	/**
	 * @brief rotate by a value
	 * 
	 * @param rotation value to rotate by
	 */
	void rotate(const glm::vec3& rotation, const RotationOrderYXZ&);
	/**
	 * @brief rotate by a value
	 * 
	 * @param rotation value to rotate by
	 */
	void rotate(const glm::vec3& rotation, const RotationOrderYZX&);
	/**
	 * @brief rotate by a value
	 * 
	 * @param rotation value to rotate by
	 */
	void rotate(const glm::vec3& rotation, const RotationOrderZXY&);
	/**
	 * @brief rotate by a value
	 * 
	 * @param rotation value to rotate by
	 */
	void rotate(const glm::vec3& rotation, const RotationOrderZYX&);

	/**
	 * @brief rotate by a value
	 * 
	 * @param x x value
	 * @param y y value
	 * @param z z value
	 */
	void rotate(const float& x, const float& y, const float& z, const RotationOrderXYZ&);
	/**
	 * @brief rotate by a value
	 * 
	 * @param x x value
	 * @param y y value
	 * @param z z value
	 */
	void rotate(const float& x, const float& y, const float& z, const RotationOrderXZY&);
	/**
	 * @brief rotate by a value
	 * 
	 * @param x x value
	 * @param y y value
	 * @param z z value
	 */
	void rotate(const float& x, const float& y, const float& z, const RotationOrderYXZ&);
	/**
	 * @brief rotate by a value
	 * 
	 * @param x x value
	 * @param y y value
	 * @param z z value
	 */
	void rotate(const float& x, const float& y, const float& z, const RotationOrderYZX&);
	/**
	 * @brief rotate by a value
	 * 
	 * @param x x value
	 * @param y y value
	 * @param z z value
	 */
	void rotate(const float& x, const float& y, const float& z, const RotationOrderZXY&);
	/**
	 * @brief rotate by a value
	 * 
	 * @param x x value
	 * @param y y value
	 * @param z z value
	 */
	void rotate(const float& x, const float& y, const float& z, const RotationOrderZYX&);

	/**
	 * @brief wrapper around the tagged rotation functions, defaults to ZYX.
	 * @brief Just override the function if you want to default to another rotation order
	 * 
	 * @param rotation value to rotate by
	 */
	void rotate(const glm::vec3& rotation) {
		rotate(rotation, RotationOrderZYX{});
	}
	/**
	 * @brief wrapper around the tagged rotation functions, defaults to ZYX. 
	 * @brief Just override the function if you want to default to another rotation order
	 * 
	 * @param x x value
	 * @param y y value
	 * @param z z value
	 */
	void rotate(const float& x, const float& y, const float& z) {
		rotate(x, y, z, RotationOrderZYX{});
	}
	/**
	 * @brief rotate the x axis
	 * 
	 * @param x x rotation
	 */
	void rotate_x(const float& x) {
		m_localTransformMatrix = glm::rotate(m_localTransformMatrix, x, {1.0f, 0.0f, 0.0f});
		m_dirty = true;
	}
	/**
	 * @brief rotate the y axis
	 * 
	 * @param y y rotation 
	 */
	void rotate_y(const float& y) {
		m_localTransformMatrix = glm::rotate(m_localTransformMatrix, y, {0.0f, 1.0f, 0.0f});
		m_dirty = true;
	}
	/**
	 * @brief rotate the z axis
	 * 
	 * @param z z rotation
	 */
	void rotate_z(const float& z) {
		m_localTransformMatrix = glm::rotate(m_localTransformMatrix, z, {0.0f, 0.0f, 1.0f});
		m_dirty = true;
	}

	/**
	 * @brief scale transform by a value
	 * 
	 * @param scale value to scale by
	 */
	void scale(const glm::vec3& scale) {
		m_localTransformMatrix = glm::scale(m_localTransformMatrix, scale);
		m_dirty = true;
	}
	/**
	 * @brief scale transform by a value
	 * 
	 * @param x x value
	 * @param y y value
	 * @param z z value
	 */
	void scale(const float& x, const float& y, const float& z) {
		m_localTransformMatrix = glm::scale(m_localTransformMatrix, {x, y, z});
		m_dirty = true;
	}
	/**
	 * @brief scale the x axis
	 * 
	 * @param x x scale
	 */
	void scale_x(const float& x) {
		m_localTransformMatrix = glm::scale(m_localTransformMatrix, {x, 0.0f, 0.0f});
		m_dirty = true;
	}
	/**
	 * @brief scale the y axis
	 * 
	 * @param y y scale
	 */
	void scale_y(const float& y) {
		m_localTransformMatrix = glm::scale(m_localTransformMatrix, {0.0f, y, 0.0f});
		m_dirty = true;
	}
	/**
	 * @brief scale the z axis
	 * 
	 * @param z z scale
	 */
	void scale_z(const float& z) {
		m_localTransformMatrix = glm::scale(m_localTransformMatrix, {0.0f, 0.0f, z});
		m_dirty = true;
	}
	
	/**
	 * @brief look at a target
	 * 
	 * @param target target translation
	 * @param up up vector, default z axis
	 */
	void look_at(const glm::vec3& target, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f));
	/**
	 * @brief move to a new position and look at an object from there
	 * 
	 * @param translation translation
	 * @param target target translation
	 * @param up up vector
	 */
	void look_at_from_position(const glm::vec3& translation, const glm::vec3& target, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f));
	/**
	 * @brief look at a target from a new translation
	 * 
	 * @param translation translation
	 * @param target target translation
	 * @param up up vector
	 */
	void look_at_from_translation(const glm::vec3& translation, const glm::vec3& target, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f)) {
		m_localTransformMatrix = glm::lookAt(translation, target, up);
		m_dirty = true;
	}
	
	/**
	 * @brief set the translation of the matrix
	 * 
	 * @param translation new translation
	 */
	void set_translation(const glm::vec3& translation);
	/**
	 * @brief set the rotation of the matrix
	 * 
	 * @param rotation new rotation
	 */
	void set_rotation(const glm::vec3& rotation);
	/**
	 * @brief set the scale of the matrix
	 * 
	 * @param scale new scale
	 */
	void set_scale(const glm::vec3& scale);
	
	/**
	 * @brief get the local translation
	 * 
	 * @return constexpr glm::vec3
	 */
	NODISCARD constexpr glm::vec3 translation() noexcept;
	/**
	 * @brief get the local rotation
	 * 
	 * @return constexpr glm::vec3
	 */
	NODISCARD constexpr glm::vec3 rotation() noexcept;
	/**
	 * @brief get the local scale
	 * 
	 * @return constexpr glm::vec3
	 */
	NODISCARD constexpr glm::vec3 scale() noexcept;
	/**
	 * @brief get the local matrix
	 * 
	 * @return constexpr glm::mat4
	 */
	NODISCARD constexpr glm::mat4 localTransformMatrix() const noexcept { 
		return m_localTransformMatrix; 
	}

private:
	glm::vec3 m_translation, m_scale, m_rotation;
	glm::mat4 m_localTransformMatrix;

	bool m_dirty = false;

	friend class Spatial;
};

} // namespace lyra
