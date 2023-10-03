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

#include <Common/Common.h>

#include <EntitySystem/Entity.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <Math/LyraMath.h>

namespace lyra {

class Transform : public ComponentBase {
public:
	/**
	 * @brief construct a transform component
	 * 
	 * @param translation translation of the object
	 * @param orientation orientation of the object
	 * @param scale scale of the object 
	 */
	constexpr Transform(
		const glm::vec3& translation = glm::vec3(0.0f), 
		const glm::quat& orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f), 
		const glm::vec3& scale = glm::vec3(1.0f)
	) : translation(translation), orientation(orientation), scale(scale), m_dirty(true) { }
	/**
	 * @brief construct a transform component
	 * 
	 * @param translation translation of the object
	 * @param axis axis to rotate the angle around
	 * @param angle angle to rotate the object by
	 * @param scale scale of the object 
	 */
	constexpr Transform(
		const glm::vec3& translation,
		const glm::vec3& axis,
		const float32& angle,
		const glm::vec3& scale
	) : translation(translation), orientation(glm::rotate(orientation, angle, axis)), scale(scale), m_dirty(true) {
	}
	
	/**
	 * @brief rotate the transform around an axis
	 * 
	 * @param axis axis to rotate around
	 * @param angle angle of the rotation
	 */
	void rotate(const glm::vec3& axis, const float32& angle) {
		orientation = glm::rotate(orientation, angle, axis);
		m_dirty = true;
	}
	/**
	 * @brief rotate the transform around an unnormalized axis
	 * 
	 * @param axis axis to rotate around
	 * @param angle angle of the rotation
	 */
	void normalizeAndRotate(const glm::vec3& axis, const float32& angle) {
		orientation = glm::rotate(orientation, angle, glm::normalize(axis));
		m_dirty = true;
	}
	/**
	 * @brief rotate the transform around the x axis
	 * 
	 * @param angle angle of the rotation
	 */
	void rotate_x(const float32& angle) {
		orientation = glm::rotate(orientation, angle, {1.0f, 0.0f, 0.0f});
		m_dirty = true;
	}
	/**
	 * @brief rotate the transform around the y axis
	 * 
	 * @param angle angle of the rotation
	 */
	void rotate_y( const float32& angle) {
		orientation = glm::rotate(orientation, angle, {0.0f, 1.0f, 0.0f});
		m_dirty = true;
	}
	/**
	 * @brief rotate the transform around the y axis
	 * 
	 * @param angle angle of the rotation
	 */
	void rotate_z(const float32& angle) {
		orientation = glm::rotate(orientation, angle, {0.0f, 0.0f, 1.0f});
		m_dirty = true;
	}
	
	/**
	 * @brief look at a target
	 * 
	 * @param target target direction
	 * @param up up vector, defaults to the  z axis
	 */
	void look_at(const glm::vec3& target, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f)) {
		orientation = glm::quatLookAt(target, up);
		m_dirty = true;
	}

	/**
	 * @brief get the front vector of the transform
	 * 
	 * @return glm::vec3
	 */
	NODISCARD glm::vec3 front() const {
		return glm::rotate(orientation, {1.0f, 0.0f, 0.0f});
	}
	/**
	 * @brief get the back vector of the transform
	 * 
	 * @return glm::vec3
	 */
	NODISCARD glm::vec3 back() const {
		return glm::rotate(orientation, {-1.0f, 0.0f, 0.0f});
	}
	/**
	 * @brief get the left vector of the transform
	 * 
	 * @return glm::vec3
	 */
	NODISCARD glm::vec3 left() const {
		return glm::rotate(orientation, {0.0f, 1.0f, 0.0f});
	}
	/**
	 * @brief get the right vector of the transform
	 * 
	 * @return glm::vec3
	 */
	NODISCARD glm::vec3 right() const {
		return glm::rotate(orientation, {0.0f, -1.0f, 0.0f});
	}
	/**
	 * @brief get the up vector of the transform
	 * 
	 * @return glm::vec3
	 */
	NODISCARD glm::vec3 up() const {
		return glm::rotate(orientation, {0.0f, 0.0f, 1.0f});
	}
	/**
	 * @brief get the down vector of the transform
	 * 
	 * @return glm::vec3
	 */
	NODISCARD glm::vec3 down() const {
		return glm::rotate(orientation, {0.0f, 0.0f, -1.0f});
	}

	/**
	 * @brief get the global rotation in euler angles
	 * 
	 * @return glm::vec3
	 */
	NODISCARD glm::vec3 local_rotation() const {
		return glm::eulerAngles(orientation);
	}
	/**
	 * @brief get the global orientation
	 * 
	 * @return glm::quat 
	 */
	NODISCARD glm::quat global_orientation() const {
		return orientation * ((m_entity->parent()) ? m_entity->parent()->component<Transform>()->orientation : glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
	}
	/**
	 * @brief get the global rotation in euler angles
	 * 
	 * @return glm::vec3
	 */
	NODISCARD glm::vec3 global_rotation() const {
		return glm::eulerAngles(global_orientation());
	}
	/**
	 * @brief get the global translation
	 * 
	 * @return glm::vec3 
	 */
	NODISCARD glm::vec3 global_translation() const {
		// this HAS got to be incredibly slow
		auto parent = m_entity->parent();
		auto parentTransform = m_entity->parent()->component<Transform>();
		return glm::rotate(((parent) ? parentTransform->orientation : glm::quat(1.0f, 0.0f, 0.0f, 0.0f)), translation) + parentTransform->global_translation();
	}
	/**
	 * @brief get the global scale
	 * 
	 * @return glm::vec3 
	 */
	NODISCARD glm::vec3 global_scale() const {
		return orientation * ((m_entity->parent()) ? m_entity->parent()->component<Transform>()->scale : glm::vec3(0.0f));
	}

	/**
	 * @brief get the local transformation matrix
	 * 
	 * @return glm::mat4 
	 */
	NODISCARD constexpr glm::mat4 local_transform() {
		if (m_dirty) { // if dirty, recalculate the local transformation matrix
			m_localTransform = glm::toMat4(orientation);
			m_localTransform = glm::translate(m_localTransform, translation);
			m_localTransform = glm::scale(m_localTransform, scale);
			m_dirty = false;
		}
		return m_localTransform;
	}
	/**
	 * @brief get the global transformation matrix
	 * 
	 * @return glm::mat4 
	 */
	NODISCARD constexpr glm::mat4 global_transform() {
		return local_transform() * ((m_entity->parent()) ? m_entity->parent()->component<Transform>()->global_transform() : glm::mat4(1.0f));
	}

	glm::quat orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 translation;
	glm::vec3 scale;

private:
	glm::mat4 m_localTransform = glm::mat4(1.0f);

	bool m_dirty = false;

	friend class Spatial;
};

} // namespace lyra
