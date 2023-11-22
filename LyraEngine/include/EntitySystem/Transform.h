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
	constexpr Transform(
		const glm::vec3& translation = glm::vec3(0.0f), 
		const glm::quat& orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f), 
		const glm::vec3& scale = glm::vec3(1.0f)
	) : translation(translation), orientation(orientation), scale(scale), m_dirty(true) { }
	constexpr Transform(
		const glm::vec3& translation,
		const glm::vec3& axis,
		float32 angle,
		const glm::vec3& scale
	) : translation(translation), orientation(glm::rotate(orientation, angle, axis)), scale(scale), m_dirty(true) { }
	
	void rotate(const glm::vec3& axis, float32 angle) {
		orientation = glm::rotate(orientation, angle, axis);
		m_dirty = true;
	}
	void normalizeAndRotate(const glm::vec3& axis, float32 angle) {
		orientation = glm::rotate(orientation, angle, glm::normalize(axis));
		m_dirty = true;
	}
	void rotate_x(float32 angle) {
		orientation = glm::rotate(orientation, angle, {1.0f, 0.0f, 0.0f});
		m_dirty = true;
	}
	void rotate_y( float32 angle) {
		orientation = glm::rotate(orientation, angle, {0.0f, 1.0f, 0.0f});
		m_dirty = true;
	}
	void rotate_z(float32 angle) {
		orientation = glm::rotate(orientation, angle, {0.0f, 0.0f, 1.0f});
		m_dirty = true;
	}
	
	void look_at(const glm::vec3& target, const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f)) {
		orientation = glm::quatLookAt(target, up);
		m_dirty = true;
	}

	NODISCARD glm::vec3 front() const {
		return glm::rotate(orientation, {1.0f, 0.0f, 0.0f});
	}
	NODISCARD glm::vec3 back() const {
		return glm::rotate(orientation, {-1.0f, 0.0f, 0.0f});
	}
	NODISCARD glm::vec3 left() const {
		return glm::rotate(orientation, {0.0f, 1.0f, 0.0f});
	}
	NODISCARD glm::vec3 right() const {
		return glm::rotate(orientation, {0.0f, -1.0f, 0.0f});
	}
	NODISCARD glm::vec3 up() const {
		return glm::rotate(orientation, {0.0f, 0.0f, 1.0f});
	}
	NODISCARD glm::vec3 down() const {
		return glm::rotate(orientation, {0.0f, 0.0f, -1.0f});
	}

	NODISCARD glm::vec3 local_rotation() const {
		return glm::eulerAngles(orientation);
	}
	NODISCARD glm::quat global_orientation() const {
		return orientation * ((m_entity->parent()) ? m_entity->parent()->component<Transform>()->orientation : glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
	}
	NODISCARD glm::vec3 global_rotation() const {
		return glm::eulerAngles(global_orientation());
	}
	NODISCARD glm::vec3 global_translation() const {
		auto parent = m_entity->parent();

		if (parent) {
			auto parentTransform = parent->component<Transform>();
			return glm::rotate(parentTransform->orientation, translation) + parentTransform->global_translation();
		} else {
			return glm::rotate(glm::quat(1.0f, 0.0f, 0.0f, 0.0f), translation);
		}
	}
	NODISCARD glm::vec3 global_scale() const {
		return orientation * ((m_entity->parent()) ? m_entity->parent()->component<Transform>()->scale : glm::vec3(0.0f));
	}

	NODISCARD constexpr glm::mat4 local_transform() {
		if (m_dirty) {
			m_localTransform = glm::toMat4(orientation);
			m_localTransform = glm::translate(m_localTransform, translation);
			m_localTransform = glm::scale(m_localTransform, scale);
			m_dirty = false;
		}
		return m_localTransform;
	}
	NODISCARD constexpr glm::mat4 global_transform() {
		return local_transform() * ((m_entity->parent()) ? m_entity->parent()->component<Transform>()->global_transform() : glm::mat4(1.0f));
	}

	glm::vec3 translation;
	glm::quat orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 scale;

private:
	glm::mat4 m_localTransform = glm::mat4(1.0f);

	bool m_dirty = false;

	friend class Spatial;
};

} // namespace lyra
