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

#include <utility>
#include <string>
#include <string_view>
#include <unordered_map>

namespace lyra {

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
	Node(
		pointer self, 
		std::string_view name,
		reference parent
	) noexcept : m_name(name), m_self(self) { parent.insert_child(self); }
	Node(
		pointer self,
		std::string_view name,
		movable parent
	) noexcept : m_name(name), m_self(self) { parent.insert_child(self); }
	Node(
		pointer self,
		std::string_view name,
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
	DEPRECATED void swap(map& other) noexcept { 
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

	iterator find(std::string_view name) { 
		return m_children.find(name); 
	}
	const_iterator find(std::string_view name) const { 
		return m_children.find(name); 
	}
	bool contains(std::string_view name) const { 
		return m_children.contains(name); 
	}

	const_pointer const operator[](std::string_view name) const { return m_children.at(name); }
	pointer operator[](std::string_view name) { return m_children.at(name); }
	const_pointer const operator/(std::string_view name) const { return m_children.at(name); }
	pointer operator/(std::string_view name) { return m_children.at(name); }

	NODISCARD size_t size() const noexcept { return m_children.size(); }
	NODISCARD std::string name() const noexcept { return m_name; }
	NODISCARD const_pointer const parent() const noexcept { return m_parent; }

protected:
	std::string m_name = "Node";

	pointer m_self;
	pointer m_parent = nullptr;
	map m_children;
};

} // namespace lyra
