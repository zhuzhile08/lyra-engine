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

#include <string>
#include <unordered_map>

namespace lyra {

/**
 * @brief an implementation for an intrusive multi linked list
 */
class Node {
public:
	using map_type = std::unordered_map<std::string, Node*>;
	using iterator_type = typename map_type::iterator;
	using const_iterator_type = typename map_type::const_iterator;
	using iterator_pair_type = std::pair<iterator_type, bool>;

	Node() = default;
	/**
	 * @brief construct a game object
	 *
	 * @param name name of the object
	 */
	Node(
		const char* name = "Game Object"
	) noexcept : m_name(name) { }
	/**
	 * @brief construct a game object
	 *
	 * @param parent parent Node of the object
	 * @param name name of the object
	 */
	Node(
		Node& parent,
		const char* name = "Game Object"
	) noexcept : m_name(name) { parent.insert_child(this); }
	/**
	 * @brief construct a game object
	 *
	 * @param parent parent Node of the object
	 * @param name name of the object
	 */
	Node(
		Node&& parent,
		const char* name = "Game Object"
	) noexcept : m_name(name) { parent.insert_child(this); }

	/**
	 * @brief clear the contents of the children
	 */
	void clear() noexcept { m_children.clear(); }

	/**
	 * @brief insert a child object into the internal map
	 *
	 * @param newChild child object to insert
	 * 
	 * @return lyra::Node::iterator_pair_type 
	 */
	iterator_pair_type insert_child(Node& child) {
		child.m_parent = this;
		return m_children.insert({child.m_name, &child});
	}
	/**
	 * @brief insert a child object into the internal map
	 *
	 * @param newChild child object to insert
	 * 
	 * @return lyra::Node::iterator_pair_type 
	 */
	iterator_pair_type insert_child(Node&& child) {
		child.m_parent = this;
		return m_children.insert({child.m_name, &child});
	}
	/**
	 * @brief insert a child object into the internal map
	 *
	 * @param newChild child object to insert
	 * 
	 * @return lyra::Node::iterator_pair_type 
	 */
	iterator_pair_type insert_child(Node* child) {
		child->m_parent = this;
		return m_children.insert({child->m_name, child});
	}

	/**
	 * @brief add a game object to another game object
	 *
	 * @param newParent the new parent of the Node
	 */
	void insert_behind(Node& parent) { 
		parent.insert_child(this);
	}
	/**
	 * @brief add a game object to another game object
	 *
	 * @param newParent the new parent of the Node
	 */
	void insert_behind(Node&& parent) { 
		parent.insert_child(this);
	}
	/**
	 * @brief add a game object to another game object
	 *
	 * @param newParent the new parent of the Node
	 */
	void insert_behind(Node* parent) { 
		parent->insert_child(this);
	}

	/**
	 * @brief erase a element at a specified position
	 * 
	 * @param pos iterator pointing to the position to erase
	 * 
	 * @return iterator_type 
	 */
	iterator_type erase(iterator_type pos) { return m_children.erase(pos); }
	/**
	 * @brief erase a element at a specified position
	 * 
	 * @param pos iterator pointing to the position to erase
	 * 
	 * @return iterator_type 
	 */
	iterator_type erase(const_iterator_type pos) { return m_children.erase(pos); }
	/**
	 * @brief erase a element between two specified positions
	 * 
	 * @param first first element to erase
	 * @param last last element to erase
	 * 
	 * @return iterator_type 
	 */
	iterator_type erase(const_iterator_type first, const_iterator_type last) { return m_children.erase(first, last); }

	/**
	 * @brief swap the children of this node with another
	 * 
	 * @param other node to swap it's children with
	 */
	void swap(Node& other) noexcept { m_children.swap(other.m_children); }
	/**
	 * @brief swap the children of this node with the contents of a raw unordered map
	 * 
	 * @param other map to swap with
	 */
	[[deprecated]] void swap(map_type& other) noexcept { m_children.swap(other); }

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
	 * @return const Node* const
	 */
	const Node* const operator[](const std::string& name) const { return m_children.at(name); }
	/**
	 * @brief get the child by name via the [] operator
	 *
	 * @param name name of the child to find
	 * 
	 * @return Node*
	 */
	Node* operator[](const std::string& name) { return m_children.at(name); }
	/**
	 * @brief get the child by name via the / operator
	 *
	 * @param name name of the child to find
	 * 
	 * @return const Node* const
	 */
	const Node* const operator/(const std::string& name) const { return m_children.at(name); }
	/**
	 * @brief get the child by name via the / operator
	 *
	 * @param name name of the child to find
	 * 
	 * @return Node* const
	 */
	Node* operator/(const std::string& name) { return m_children.at(name); }

	/**
	 * @brief find an element with a name, if it does not exist yet, create one with that name
	 * 
	 * @param name name of element to find or create
	 * 
	 * @return iterator_type 
	 */
	iterator_type find(const std::string& name)	{ return m_children.find(name); }
	/**
	 * @brief find an element with a name, if it does not exist yet, create one with that name
	 * 
	 * @param name name of element to find or create
	 * 
	 * @return const_iterator_type 
	 */
	const_iterator_type find(const std::string& name) const { return m_children.find(name); }

	/**
	 * @brief check if a child with the name exists in the internal map
	 * 
	 * @param name name of the child to find
	 * 
	 * @return true if found
	 * @return false if not found
	 */
	[[nodiscard]] bool contains(const std::string& name) const { return m_children.contains(name); }

	/**
	 * @brief get the name
	 *
	 * @return std::string
	 */
	[[nodiscard]] std::string name() const noexcept { return m_name; }
	/**
	 * @brief get the parent
	 *
	 * @return Node* const
	 */
	[[nodiscard]] const Node* const parent() const noexcept { return m_parent; }

protected:
	std::string m_name = "Node";

	Node* m_parent = nullptr;
	map_type m_children;
};

} // namespace lyra
