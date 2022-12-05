/*************************
 * @file colliders.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief A class containing all basic collision shapes
 * 
 * @date 2022-10-10
 * 
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <lyra.h>

#include <glm.hpp>
#include <Jolt/Jolt.h>

#include <nodes/node.h>
#include <nodes/spatial.h>

#include <nodes/physics/rigidbody.h>

namespace lyra {

/**
 * @brief a collider base class for collision of basic objects
 */
class Collider : public Spatial{
public:
	Collider(
		Rigidbody* parent,
		const char* name = "BaseCollider",
		const uint32 tag = 0,
		const bool active = true,
		const glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		const glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f),
		const glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f),
		const RotationOrder rotationOrder = RotationOrder::ROTATION_ZYX
	) : Spatial(name, parent, true, tag, position, rotation, scale, rotationOrder), 
		_parent(parent), 
		_active(active) { }

private:
	bool _active;
	const Rigidbody* _parent;
};

class BoxCollider : Collider {
public:
	BoxCollider(
		Rigidbody* parent,
		const char* name = "BoxCollider",
		const glm::vec3 dimensions = glm::vec3(1.0f, 1.0f, 1.0f),
		const bool active = true,
		const uint32 tag = 0,
		const glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		const glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f),
		const glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f),
		const RotationOrder rotationOrder = RotationOrder::ROTATION_ZYX
	);

private:
	glm::vec3 _dimensions;
};

} // namespace lyra
