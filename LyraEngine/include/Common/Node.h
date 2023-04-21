/*************************
 * @file Node.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief Base node class with forward declarations
 *
 * @date 2022-22-06
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <utility>
#include <string>
#include <string_view>
#include <unordered_map>

namespace lyra {

/**
 * @brief an implementation for an intrusive multi linked list
 */
template <class Ty> class Node {
public:
	using value_type = Ty;
	using const_value = const value_type;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = value_type*;
	using const_pointer = const pointer;
	using movable = value_type&&;
	using map = std::unordered_map<std::string, pointer>;
	using iterator = typename map::iterator;
	using const_iterator = typename map::const_iterator;
	using iterator_pair = std::pair<iterator, bool>;

	Node() = default;
	/**
	 * @brief construct a game object
	 *
	 * @param self pointer to "self", aka the derived type
	 * @param name name of the object
	 * @param parent parent Node of the object
	 */
	Node(
		pointer self, 
		std::string_view name,
		reference parent
	) noexcept : m_name(name), m_self(self) { parent.insert_child(self); }
	/**
	 * @brief construct a game object
	 *
	 * @param self pointer to "self", aka the derived type
	 * @param name name of the object
	 * @param parent parent Node of the object
	 */
	Node(
		pointer self,
		std::string_view name,
		movable parent
	) noexcept : m_name(name), m_self(self) { parent.insert_child(self); }
	/**
	 * @brief construct a game object
	 *
	 * @param name name of the object
	 * @param parent parent Node of the object
	 */
	Node(
		pointer self,
		std::string_view name,
		pointer parent = nullptr
	) noexcept : m_name(name), m_self(self) { if (parent) parent->insert_child(self); }

	/**
	 * @brief clear the contents of the children
	 */
	void clear() noexcept { m_children.clear(); }

	/**
	 * @brief insert a child object into the internal map
	 *
	 * @param newChild child object to insert
	 * 
	 * @return lyra::Node::iterator_pair 
	 */
	iterator_pair insert_child(reference child) {
		child.m_parent = m_self;
		return m_children.insert({child.m_name, &child});
	}
	/**
	 * @brief insert a child object into the internal map
	 *
	 * @param newChild child object to insert
	 * 
	 * @return lyra::Node::iterator_pair 
	 */
	iterator_pair insert_child(movable child) {
		child.m_parent = m_self;
		return m_children.insert({child.m_name, &child});
	}
	/**
	 * @brief insert a child object into the internal map
	 *
	 * @param newChild child object to insert
	 * 
	 * @return lyra::Node::iterator_pair 
	 */
	iterator_pair insert_child(pointer child) {
		child->m_parent = m_self;
		return m_children.insert({child->m_name, child});
	}

	/**
	 * @brief add a game object to another game object
	 *
	 * @param newParent the new parent of the Node
	 */
	void insert_behind(reference parent) { 
		parent.insert_child(this);
	}
	/**
	 * @brief add a game object to another game object
	 *
	 * @param newParent the new parent of the Node
	 */
	void insert_behind(movable parent) { 
		parent.insert_child(this);
	}
	/**
	 * @brief add a game object to another game object
	 *
	 * @param newParent the new parent of the Node
	 */
	void insert_behind(pointer parent) { 
		parent->insert_child(this);
	}

	/**
	 * @brief erase a element at a specified position
	 * 
	 * @param pos iterator pointing to the position to erase
	 * 
	 * @return lyra::Node::iterator 
	 */
	iterator erase(iterator pos) { return m_children.erase(pos); }
	/**
	 * @brief erase a element at a specified position
	 * 
	 * @param pos iterator pointing to the position to erase
	 * 
	 * @return lyra::Node::iterator 
	 */
	iterator erase(const_iterator pos) { return m_children.erase(pos); }
	/**
	 * @brief erase a element between two specified positions
	 * 
	 * @param first first element to erase
	 * @param last last element to erase
	 * 
	 * @return lyra::Node::iterator 
	 */
	iterator erase(const_iterator first, const_iterator last) { return m_children.erase(first, last); }

	/**
	 * @brief swap the children of this node with another
	 * 
	 * @param other node to swap it's children with
	 */
	void swap(reference other) noexcept { m_children.swap(other.m_children); }
	/**
	 * @brief swap the children of this node with the contents of a raw unordered map
	 * 
	 * @param other map to swap with
	 */
	[[deprecated]] void swap(map& other) noexcept { m_children.swap(other); }

	/**
	 * @brief check if container is empty
	 * 
	 * @return true if container is empty
	 * @return false if container is not empty
	 */
	[[nodiscard]] bool empty() const noexcept { return m_children.empty(); }
	/**
	 * @brief check if container is empty or has children
	 * 
	 * @return true if container is empty
	 * @return false if container is not empty
	 */
	[[nodiscard]] bool no_children() const noexcept { return m_children.empty(); }
	/**
	 * @brief check if container is empty
	 * 
	 * @return true if container is empty
	 * @return false if container is not empty
	 */
	operator bool() const noexcept { return m_children.empty(); }

	/**
	 * @brief get the current size of the container
	 * 
	 * @return size_t 
	 */
	[[nodiscard]] size_t size() const noexcept { return m_children.size(); }

	/**
	 * @brief get the child by name via the [] operator
	 *
	 * @param name name of the child to find
	 * 
	 * @return lyra::Node::const_pointer const
	 */
	const_pointer const operator[](std::string_view name) const { return m_children.at(name); }
	/**
	 * @brief get the child by name via the [] operator
	 *
	 * @param name name of the child to find
	 * 
	 * @return lyra::Node::pointer
	 */
	pointer operator[](std::string_view name) { return m_children.at(name); }
	/**
	 * @brief get the child by name via the / operator
	 *
	 * @param name name of the child to find
	 * 
	 * @return lyra::Node::const_pointer const
	 */
	const_pointer const operator/(std::string_view name) const { return m_children.at(name); }
	/**
	 * @brief get the child by name via the / operator
	 *
	 * @param name name of the child to find
	 * 
	 * @return lyra::Node::pointer
	 */
	pointer operator/(std::string_view name) { return m_children.at(name); }

	/**
	 * @brief find an element with a name, if it does not exist yet, create one with that name
	 * 
	 * @param name name of element to find or create
	 * 
	 * @return lyra::Node::iterator 
	 */
	iterator find(std::string_view name)	{ return m_children.find(name); }
	/**
	 * @brief find an element with a name, if it does not exist yet, create one with that name
	 * 
	 * @param name name of element to find or create
	 * 
	 * @return lyra::Node::const_iterator 
	 */
	const_iterator find(std::string_view name) const { return m_children.find(name); }

	/**
	 * @brief check if a child with the name exists in the internal map
	 * 
	 * @param name name of the child to find
	 * 
	 * @return true if found
	 * @return false if not found
	 */
	[[nodiscard]] bool contains(std::string_view name) const { return m_children.contains(name); }

	/**
	 * @brief get the name
	 *
	 * @return std::string
	 */
	[[nodiscard]] std::string name() const noexcept { return m_name; }
	/**
	 * @brief get the parent
	 *
	 * @return const lyra::Node::const_pointer const
	 */
	[[nodiscard]] const_pointer const parent() const noexcept { return m_parent; }

protected:
	std::string m_name = "Node";

	pointer m_self;
	pointer m_parent = nullptr;
	map m_children;
};

} // namespace lyra
