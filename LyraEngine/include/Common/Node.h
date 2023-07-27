/*************************
 * @file Node.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 *
 * @brief Base node class
 *
 * @date 2022-22-06
 *
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <Common/Common.h>

#include <type_traits>
#include <utility>
#include <functional>

namespace lyra {

// Multi linked list implementation in the style of standard library containers
// Creation inspired by Godot's Node system, therefore also called node
// Can be used as a normal class member, but more recommended to use it like this to provide a better interface:
// class Foo : Node<Foo> {}
// Therefore also the pointers to "self"
template <class Ty, class Key = std::string, class Hash = std::hash<Key>, template<class...> class Container = std::unordered_map> class Node {
public:
	using value_type = Ty;
	using const_value = const value_type;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = value_type*;
	using const_pointer = const pointer;
	using movable = value_type&&;
	using key_type = Key;
	using hash_function = Hash;
	using container = Container<key_type, value_type, hash_function>;
	// special check in case key is a std::string, then set the key type passed in functions to a std::string, else use a const reference
	using key_access = typename std::conditional<
		std::same_as<key_type, std::string>, 
		std::string_view, 
		const key_type&
	>::type;
	using iterator = typename container::iterator;
	using const_iterator = typename container::const_iterator;
	using iterator_pair = std::pair<iterator, bool>;

	Node() = default;
	Node(
		pointer self, 
		key_access name,
		reference parent
	) noexcept : m_name(name), m_self(self) { parent.insert_child(self); }
	Node(
		pointer self,
		key_access name,
		movable parent
	) noexcept : m_name(name), m_self(self) { parent.insert_child(self); }
	Node(
		pointer self,
		key_access name,
		pointer parent = nullptr
	) noexcept : m_name(name), m_self(self) { if (parent) parent->insert_child(self); }

	void clear() noexcept { m_children.clear(); }

	iterator_pair insert_child(reference child) {
		child.m_parent = m_self;
		return m_children.insert({child.m_name, &child});
	}
	iterator_pair insert_child(movable child) {
		child.m_parent = m_self;
		return m_children.insert({child.m_name, &child});
	}
	iterator_pair insert_child(pointer child) {
		child->m_parent = m_self;
		return m_children.insert({child->m_name, child});
	}

	void insert_behind(reference parent) { 
		parent.insert_child(this);
	}
	void insert_behind(movable parent) { 
		parent.insert_child(this);
	}
	void insert_behind(pointer parent) { 
		parent->insert_child(this);
	}

	iterator erase(iterator pos) { 
		return m_children.erase(pos); 
	}
	iterator erase(const_iterator pos) { 
		return m_children.erase(pos); 
	}
	iterator erase(const_iterator first, const_iterator last) { 
		return m_children.erase(first, last); 
	}

	void swap(reference other) noexcept { 
		m_children.swap(other.m_children); 
	}
	DEPRECATED void swap(container& other) noexcept { 
		m_children.swap(other); 
	}

	NODISCARD bool empty() const noexcept { 
		return m_children.empty(); 
	}
	NODISCARD bool no_children() const noexcept { 
		return m_children.empty(); 
	}
	operator bool() const noexcept { 
		return m_children.empty(); 
	}

	iterator find(key_access name) { 
		return m_children.find(name); 
	}
	const_iterator find(key_access name) const { 
		return m_children.find(name); 
	}
	bool contains(key_access name) const { 
		return m_children.contains(name); 
	}

	const_pointer const operator[](key_access name) const { return m_children.at(name); }
	pointer operator[](key_access name) { return m_children.at(name); }
	const_pointer const operator/(key_access name) const { return m_children.at(name); }
	pointer operator/(key_access name) { return m_children.at(name); }

	NODISCARD size_t size() const noexcept { return m_children.size(); }
	NODISCARD std::string name() const noexcept { return m_name; }
	NODISCARD const_pointer const parent() const noexcept { return m_parent; }

protected:
	std::string m_name = "Node";

	pointer m_self;
	pointer m_parent = nullptr;
	container m_children;
};

} // namespace lyra
