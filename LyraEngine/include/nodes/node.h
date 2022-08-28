/*************************
 * @file node.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief Base node class with forward declarations
 *
 * @date 2022-22-06
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <core/decl.h>

#include <unordered_map>
#include <string>

namespace lyra {

/**
 * @brief a base class for a node
 * 
 * @tparam _Ty type of children/parent
 */
template <class _Ty> class Node {
public:
	/**
	 * @brief construct a game object
	 *
	 * @param name name of the object
	 * @param parent parent Node of the object
	 * @param visible visibility of the object
	 * @param tag optional tag of the object
	 */
	Node(
		const char* name = "Game Object",
		_Ty* parent = nullptr,
		const bool visible = true,
		const uint32 tag = 0
	) noexcept : m_name(name), m_parent(parent), m_visible(visible), m_tag(tag) { init(); }

	/**
	 * @brief destroy the game object
	 */
	~Node() { for (auto& [name, child] : m_children) child->set_parent(m_parent); }

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
	void show() noexcept { m_visible = true; }
	/**
	 * @brief hide the object
	 */
	void hide() noexcept { m_visible = false; }

	/**
	 * @brief add a child object
	 *
	 * @param newChild
	 */
	void add_child(_Ty* newChild) {
		newChild->m_parent = this;
		m_children[newChild->name()] = newChild;
	}
	/**
	 * @brief add the game object to the front of a tree
	 *
	 * @param root the game object that is going to be behind this one
	 */
	void add_to_beginning(_Ty root) noexcept {
		m_parent = nullptr;
		root.m_parent = this;
	}

	/**
	 * @brief add a game object to another game object
	 *
	 * @param newParent the new parent of the Node
	 */
	void add_to(_Ty* newParent) noexcept { m_parent = newParent; }

	/**
	 * @brief add a game object between two game object
	 * @cond this only works when the parameter newParent is not at the front of a scene tree
	 *
	 * @param front the new game object that is supposed be in front
	 */
	void add_between(_Ty* front) {
		m_parent = front->m_parent;
		add_child(front);
	}

	/**
	 * @brief set the tag to a new one
	 *
	 * @param tag new tag
	 */
	void set_tag(const uint32 tag) noexcept { m_tag = tag; }
	/**
	 * @brief set the parent
	 *
	 * @param newParent the parent
	 */
	void set_parent(_Ty* newParent) noexcept { m_parent = newParent; }

	/**
	 * @brief get the child by name
	 *
	 * @param name name of the child to find
	 * @return const Node* const
	 */
	NODISCARD _Ty* const get_child_by_name(const std::string name) const { return m_children.at(m_name); }

	/**
	 * @brief get if the object is visible or not
	 *
	 * @return const bool
	*/
	NODISCARD const bool visibility() const noexcept { return m_visible; }
	/**
	 * @brief get if the object is visible or not
	 *
	 * @return const uint32_t
	*/
	NODISCARD const uint32 tag() const noexcept { return m_tag; }
	/**
	 * @brief get the name
	 *
	 * @return const std::string
	 */
	NODISCARD const std::string name() const noexcept { return m_name; }
	/**
	 * @brief get the children
	 *
	 * @return const std::unordered_map <std::string, Node*>* cosnt
	 */
	NODISCARD const std::unordered_map <std::string, Node*>* const children() const noexcept { return &m_children; }
	/**
	 * @brief get the parent
	 *
	 * @return const _Ty* const
	 */
	NODISCARD const _Ty* const parent() const noexcept { return m_parent; }

protected:
	bool m_visible = true;

	uint32 m_tag = 0; // you can go and define some tags yourself, just remember that 0 is always an undefined tag

	std::string m_name = "Node";

	_Ty* m_parent = nullptr;
	std::unordered_map <std::string, _Ty*> m_children;
};

// nodes

class Spatial;

class Mesh;
class MeshRenderer;
class Text;
class TextRenderer;

class RigidBody;
class Collider;
class MeshCollider;
class Joint;
class Cloth;
class Raycast;
class Spatial;

class Camera;
class Light;
class LightProbe;
class Skybox;
class MoviePlayer;

class ParticleSystem;
class LineRenderer;

class Animation;
class Animator;

class AudioSource;
class AudioListener;
class AudioFilter;

} // namespace lyra
