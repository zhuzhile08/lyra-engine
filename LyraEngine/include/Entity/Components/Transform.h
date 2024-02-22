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

#include <Entity/Entity.h>
#include <Entity/ECS.h>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <Math/LyraMath.h>

namespace lyra {

class Transform : public BasicComponent {
public:
	GLM_CONSTEXPR Transform(
		const glm::vec3& translation = glm::vec3(0.0f), 
		const glm::quat& orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f), 
		const glm::vec3& scale = glm::vec3(1.0f)
	) : translation(translation), orientation(orientation), scale(scale), m_dirty(true) { }
	GLM_CONSTEXPR Transform(
		const glm::vec3& translation,
		const glm::vec3& axis,
		float32 angle,
		const glm::vec3& scale
	) : translation(translation), orientation(glm::rotate(orientation, angle, axis)), scale(scale), m_dirty(true) { }
	
	void rotate(const glm::vec3& axis, float32 angle) {
		orientation = glm::rotate(orientation, angle, axis);
		m_dirty = true;
	}
	void rotate(const glm::vec3& euler) {
		orientation = orientation * glm::quat(euler);
		m_dirty = true;
	}
	void setOrientation(const glm::vec3& axis, float32 angle) {
		orientation = glm::rotate(glm::quat(1.0f, 0.0f, 0.0f, 0.0f), angle, axis);
		m_dirty = true;
	}
	void setOrientation(const glm::vec3& euler) {
		orientation = glm::quat(euler);
		m_dirty = true;
	}
	void normalizeAndRotate(const glm::vec3& axis, float32 angle) {
		orientation = glm::rotate(orientation, angle, glm::normalize(axis));
		m_dirty = true;
	}
	
	void lookAt(const glm::vec3& target, const glm::vec3& up = glm::vec3(0.0f, 0.0f, 1.0f)) {
		m_dirty = true;
		
		glm::vec3 direction = translation - target;
		float32 length = glm::length(direction);
		
		if (length < 0.0001) return;
		
		direction /= length;
		
		if (glm::abs(glm::dot(direction, up)) < 0.1)
			orientation = glm::normalize(glm::inverse(glm::quatLookAt(direction, this->up())));
		else
			orientation = glm::normalize(glm::inverse(glm::quatLookAt(direction, up)));
	}

	NODISCARD glm::vec3 forward() const {
		return glm::normalize(glm::rotate(orientation, { 0.0f, 1.0f, 0.0f }));
	}
	NODISCARD glm::vec3 left() const {
		return glm::normalize(glm::rotate(orientation, { 1.0f, 0.0f, 0.0f }));
	}
	NODISCARD glm::vec3 up() const {
		return glm::normalize(glm::rotate(orientation, { 0.0f, 0.0f, 1.0f }));
	}
	NODISCARD glm::vec3 globalForward() const {
		return glm::normalize(glm::rotate(globalOrientation(), { 0.0f, 1.0f, 0.0f }));
	}
	NODISCARD glm::vec3 globalLeft() const {
		return glm::normalize(glm::rotate(globalOrientation(), { 1.0f, 0.0f, 0.0f }));
	}
	NODISCARD glm::vec3 globalUp() const {
		return glm::normalize(glm::rotate(globalOrientation(), { 0.0f, 0.0f, 1.0f }));
	}

	NODISCARD glm::vec3 localRotation() const {
		return glm::eulerAngles(orientation);
	}
	NODISCARD glm::quat globalOrientation() const {
		return orientation * ((entity->parent()) ? entity->parent()->component<Transform>().orientation : glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
	}
	NODISCARD glm::vec3 globalRotation() const {
		return glm::eulerAngles(globalOrientation());
	}
	NODISCARD glm::vec3 globalTranslation() const {
		auto parent = entity->parent();

		if (parent) {
			auto& parentTransform = parent->component<Transform>();
			return glm::rotate(parentTransform.orientation, translation) + parentTransform.globalTranslation();
		} else {
			return glm::rotate(glm::quat(1.0f, 0.0f, 0.0f, 0.0f), translation);
		}
	}
	NODISCARD glm::vec3 globalScale() const {
		return orientation * ((entity->parent()) ? entity->parent()->component<Transform>().scale : glm::vec3(0.0f));
	}

	NODISCARD GLM_CONSTEXPR glm::mat4 localTransform() {
		if (m_dirty) {
			m_localTransform = glm::scale(glm::translate(glm::toMat4((orientation = glm::normalize(orientation))), translation), scale);
			m_dirty = false;
		}
		return m_localTransform;
	}
	NODISCARD GLM_CONSTEXPR glm::mat4 globalTransform() {
		return localTransform() * ((entity->parent()) ? entity->parent()->component<Transform>().globalTransform() : glm::mat4(1.0f));
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
