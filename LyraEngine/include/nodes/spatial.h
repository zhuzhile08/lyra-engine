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
		Script* script = new Script,
		const char* name = "Game Object",
		Spatial* parent = nullptr,
		const bool& visible = true,
		const uint32& tag = 0,
		const Transform& transform = Transform()
	) noexcept : Node<Spatial>(name, parent, visible, tag), transform(transform), m_script(script) { m_script->init(); }

	/**
	 * @brief convert the local matrix to a global one
	 *
	 * @return const lyra::glm::mat4
	*/
	NODISCARD const glm::mat4 mat_to_global() const noexcept { return transform.localTransformMatrix() * ((this->m_parent == nullptr) ? glm::mat4(0.0f) : this->m_parent->mat_to_global()); };

	Transform transform;

protected:
	SmartPointer<Script> m_script;

	/**
	 * @brief calculate the transformation matrix based on the parent rotation matrix and the current rotation
	 */
	void calculate_transform_mat();
};

// @brief https://cdn.discordapp.com/attachments/841451660059869194/1028773573290119188/MemeFeedBot.mp4

} // namespace lyra
