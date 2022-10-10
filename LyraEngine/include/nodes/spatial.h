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

#include <core/decl.h>
#include <core/util.h>

#include <nodes/script.h>
#include <nodes/node.h>

namespace lyra {

/**
 * @brief basic 3d node with positions and matrices
 */
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
	 * @param script script of the object
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
		Script<Spatial>* script = new Script<Spatial>,
		const bool visible = true,
		const uint32 tag = 0,
		const glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		const glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f),
		const glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f),
		const RotationOrder rotationOrder = RotationOrder::ROTATION_ZYX
	) noexcept : Node(name, parent, visible, tag), m_position(position), m_rotation(rotation), m_scale(scale), m_rotationOrder(rotationOrder), m_script(script) { m_script->init(); }

	/**
	 * @brief construct a transform component
	 * @brief this function is only meant for library internal use, please don't actually use this
	 * @brief but hey, Here's a good meme for finding this goofy ahh function
	 * @brief https://cdn.discordapp.com/attachments/841451660059869194/1028773573290119188/MemeFeedBot.mp4
	 *
	 * @param name name of the object
	 * @param parent parent Node of the object
	 * @param visible visibility of the object
	 * @param tag optional tag of the object
	 * @param position position of the object
	 * @param rotation rotation of the object
	 * @param scale scale of the object
	 * @param rotationOrder order of the multiplication of the rotation matricies
	 * @param script script of the object
	 */
	Spatial(
		bool,
		const char* name = "Game Object",
		Spatial* parent = nullptr,
		const bool visible = true,
		const uint32 tag = 0,
		const glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		const glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f),
		const glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f),
		const RotationOrder rotationOrder = RotationOrder::ROTATION_ZYX,
		Script<Spatial>* script = new Script<Spatial>
	) noexcept : Node(name, parent, visible, tag), m_position(position), m_rotation(rotation), m_scale(scale), m_rotationOrder(rotationOrder), m_script(script) { m_script->init(); }

	
	/**
	 * @brief move the object
	 *
	 * @param velocity velocity to move the object
	 * @param space space to translate the object in
	 */
	void translate(glm::vec3 velocity, Space space = Space::SPACE_LOCAL) { set_position(m_position + velocity, space); }
	/**
	 * @brief rotate the game object
	 *
	 * @param rotation rotation in degrees
	 * @param space space to rotate the object in
	 */
	void rotate(glm::vec3 rotation, Space space = Space::SPACE_LOCAL) {
		set_rotation(m_rotation + rotation, space);
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
	NODISCARD const glm::vec3 position_global() const noexcept { return m_position + m_parent->position_global(); };
	/**
	 * @brief get the global rotation
	 *
	 * @return const glm::vec3
	*/
	NODISCARD const glm::vec3 rotation_global() const noexcept { return m_rotation + ((m_parent == nullptr) ? glm::vec3(0.0f) : m_parent->rotation_global()); };
	/**
	 * @brief get the global scale
	 *
	 * @return const glm::vec3
	*/
	NODISCARD const glm::vec3 scale_global() const noexcept { return m_scale + ((m_parent == nullptr) ? glm::vec3(0.0f) : m_parent->scale_global()); };
	/**
	 * @brief convert the local matrix to a global one
	 *
	 * @return const glm::vec4
	*/
	NODISCARD const glm::mat4 mat_to_global() const noexcept { return m_localTransformMatrix + ((m_parent == nullptr) ? glm::mat4(0.0f) : m_parent->mat_to_global()); };

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
	void set_scale(glm::vec3 newScale) noexcept { m_scale = newScale; }

	/**
	 * @brief get the local position
	 *
	 * @return const glm::vec3
	*/
	NODISCARD const glm::vec3 position_local() const noexcept { return m_position; };
	/**
	 * @brief get the local rotation
	 *
	 * @return const glm::vec3
	*/
	NODISCARD const glm::vec3 rotation_local() const noexcept { return m_rotation; };
	/**
	 * @brief get the local scale
	 *
	 * @return const glm::vec3
	*/
	NODISCARD const glm::vec3 scale_local() const noexcept { return m_scale; };
	/**
	 * @brief get the order of rotation
	 *
	 * @return const lyra::Spatial::RotationOrder
	*/
	NODISCARD const RotationOrder rotationOrder() const noexcept { return m_rotationOrder; }

protected:
	glm::vec3 m_position = { 0.0f, 0.0f, 0.0f }, m_rotation = { 0.0f, 0.0f, 0.0f }, m_scale = { 1.0f, 1.0f, 1.0f };
	RotationOrder m_rotationOrder = RotationOrder::ROTATION_ZYX;
	glm::mat4 m_localTransformMatrix = glm::mat4(1.0f);

	LYRA_NODE_SCRIPT_MEMBER(Spatial)

private:
	/**
	 * @brief calculate the transformation matrix based on the parent rotation matrix and the current rotation
	 */
	void calculate_transform_mat();
};

} // namespace lyra
