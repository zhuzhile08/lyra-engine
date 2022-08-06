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
template <class _Tp> class Node {
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
		_Tp* parent = nullptr,
		const bool visible = true,
		const uint32 tag = 0
	) noexcept : _name(name), _parent(parent), _visible(visible), _tag(tag) { init(); }

	/**
	 * @brief destroy the game object
	 */
	~Node() { for (auto& [name, child] : _children) child->set_parent(_parent); }

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
	void show() noexcept { _visible = true; }
	/**
	 * @brief hide the object
	 */
	void hide() noexcept { _visible = false; }

	/**
	 * @brief add a child object
	 *
	 * @param newChild
	 */
	void add_child(_Tp* newChild) {
		newChild->_parent = this;
		_children[newChild->name()] = newChild;
	}
	/**
	 * @brief add the game object to the front of a tree
	 *
	 * @param root the game object that is going to be behind this one
	 */
	void add_to_beginning(_Tp root) noexcept {
		_parent = nullptr;
		root._parent = this;
	}

	/**
	 * @brief add a game object to another game object
	 *
	 * @param newParent the new parent of the Node
	 */
	void add_to(_Tp* newParent) noexcept { _parent = newParent; }

	/**
	 * @brief add a game object between two game object
	 * @cond this only works when the parameter newParent is not at the front of a scene tree
	 *
	 * @param front the new game object that is supposed be in front
	 */
	void add_between(_Tp* front) {
		_parent = front->_parent;
		add_child(front);
	}

	/**
	 * @brief set the tag to a new one
	 *
	 * @param tag new tag
	 */
	void set_tag(const uint32 tag) noexcept { _tag = tag; }
	/**
	 * @brief set the parent
	 *
	 * @param newParent the parent
	 */
	void set_parent(_Tp* newParent) noexcept { _parent = newParent; }

	/**
	 * @brief get the child by name
	 *
	 * @param name name of the child to find
	 * @return const Node* const
	 */
	[[nodiscard]] _Tp* const get_child_by_name(const std::string name) const { return _children.at(_name); }

	/**
	 * @brief get if the object is visible or not
	 *
	 * @return const bool
	*/
	[[nodiscard]] const bool visibility() const noexcept { return _visible; }
	/**
	 * @brief get if the object is visible or not
	 *
	 * @return const uint32_t
	*/
	[[nodiscard]] const uint32 tag() const noexcept { return _tag; }
	/**
	 * @brief get the name
	 *
	 * @return const std::string
	 */
	[[nodiscard]] const std::string name() const noexcept { return _name; }
	/**
	 * @brief get the children
	 *
	 * @return const std::unordered_map <std::string, Node*>* cosnt
	 */
	[[nodiscard]] const std::unordered_map <std::string, Node*>* const children() const noexcept { return &_children; }
	/**
	 * @brief get the parent
	 *
	 * @return const _Tp* const
	 */
	[[nodiscard]] const _Tp* const parent() const noexcept { return _parent; }

protected:
	bool _visible = true;

	uint32 _tag = 0; // you can go and define some tags yourself, just remember that 0 is always an undefined tag

	std::string _name = "Node";

	_Tp* _parent = nullptr;
	std::unordered_map <std::string, _Tp*> _children;
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
