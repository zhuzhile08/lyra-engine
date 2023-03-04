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

#include <lyra.h>

#include <core/smart_pointer.h>

#include <glm.hpp>

#include <math/transform.h>

#include <nodes/script.h>
#include <nodes/node.h>

namespace lyra {

/**
 * @brief basic 3d node with positions and matrices
 */
class Spatial : public Node<Spatial> {
public:
	Spatial() = default;
	/**
	 * @brief construct a transform component
	 *
	 * @param script script of the object
	 * @param name name of the object
	 * @param parent parent Node of the object
	 * @param visible visibility of the object
	 * @param tag optional tag of the object
	 * @param transform transform of the object
	 */
	Spatial(
		Script* script,
		const char* name = "Game Object",
		Spatial* parent = nullptr,
		const bool& visible = true,
		const uint32& tag = 0,
		const Transform& transform = Transform()
	) noexcept : Node<Spatial>(name, parent, visible, tag), transform(transform), m_script(script) { 
		if (script != nullptr) {
			m_script->node = this; 
			m_script->init(); 
		}
	}

	/**
	 * @brief convert the local matrix to a global one
	 *
	 * @return constexpr lyra::glm::mat4
	*/
	NODISCARD constexpr glm::mat4 mat_to_global() const noexcept { 
		if (this->m_parent != nullptr)
			return transform.localTransformMatrix() * this->m_parent->mat_to_global();
		else
			return transform.localTransformMatrix();
	};

	Transform transform;

protected:
	SmartPointer<Script> m_script;

	/**
	 * @brief calculate the transformation matrix based on the parent rotation matrix and the current rotation
	 */
	void calculate_transform_mat();
};

} // namespace lyra
