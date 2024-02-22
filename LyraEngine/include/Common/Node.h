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
#include <Common/UniquePointer.h>
#include <Common/SharedPointer.h>

#include <type_traits>
#include <utility>

#include <string>
#include <string_view>
#include <unordered_map>

namespace lyra {

template <
	class Type, 
	template<class...> class SmartPointer = UniquePointer,
	class Key = std::string, 
	class HashOrCompare = std::hash<Key>, 
	template<class...> class Container = std::unordered_map> 
class BasicNode {
public:
	using value_type = Type;
	using const_value = const value_type;
	using reference = value_type&;
	using const_reference = const value_type&;
	using movable = value_type&&;
	using pointer = value_type*;
	using const_pointer = const pointer;
	using pointer_const = const_value*;
	using smart_pointer = SmartPointer<value_type>;

	using key_type = Key;
	using key_reference = key_type&;
	using const_key_reference = const key_type&;
	using key_rvreference = key_type&&;

	using hash_function = HashOrCompare;
	using container = Container<key_type, smart_pointer, hash_function>;

	using iterator = typename container::iterator;
	using const_iterator = typename container::const_iterator;
	using iterator_pair = std::pair<iterator, bool>;

	BasicNode() = default;
	template <class NameType, class... Args> BasicNode(
		NameType&& name,
		reference parent
	) noexcept : m_name(std::forward<NameType>(name)) { parent.insert(dynamic_cast<pointer>(this)); }
	template <class NameType, class... Args> BasicNode(
		NameType&& name,
		movable parent
	) noexcept : m_name(std::forward<NameType>(name)) { parent.insert(dynamic_cast<pointer>(this)); }
	template <class NameType, class... Args> BasicNode(
		NameType&& name,
		pointer parent = nullptr
	) noexcept : m_name(std::forward<NameType>(name)) { if (parent) parent->insert(dynamic_cast<pointer>(this)); }
	BasicNode(BasicNode&&) = default;
	BasicNode(const BasicNode&) requires std::is_copy_assignable_v<smart_pointer> = default;
	virtual ~BasicNode() = default;

	BasicNode& operator=(BasicNode&&) = default;
	BasicNode& operator=(const BasicNode&) requires std::is_copy_assignable_v<smart_pointer> = default;

	reference clear() noexcept { 
		m_children.clear();
		return *this;
	}

	iterator begin() noexcept {
		return m_children.begin();
	}
	const_iterator begin() const noexcept {
		return m_children.begin();
	}
	const_iterator cbegin() const noexcept {
		return m_children.begin();
	}

	iterator end() noexcept {
		return m_children.end();
	}
	const_iterator end() const noexcept {
		return m_children.end();
	}
	const_iterator cend() const noexcept {
		return m_children.end();
	}

	reference insert(movable child) {
		child.m_parent = this;
		return *m_children.emplace(child.m_name, smart_pointer::create(std::move(child))).first->second.get();
	}
	reference insert(pointer child) {
		child->m_parent = this;
		return *m_children.emplace(child->m_name, smart_pointer(child)).first->second.get();
	}
	template <class... Args> reference insert(Args&&... args) {
		auto child = smart_pointer::create(std::forward<Args>(args)...);
		child->m_parent = this; // for extra safety
		return *m_children.emplace(child->m_name, std::move(child)).first->second.get();
	}

	template <class KeyType> reference rename(KeyType&& name) {
		auto t = dynamic_cast<pointer>(this);
		m_parent->m_children.extract(std::exchange(m_name, std::forward<KeyType>(name)));
		m_parent->m_children.emplace(m_name, smart_pointer(t));
		return *t;
	}

	reference erase(iterator pos) { 
		return m_children.erase(pos); 
		return *dynamic_cast<pointer>(this);
	}
	reference erase(const_iterator pos) { 
		return m_children.erase(pos); 
		return *dynamic_cast<pointer>(this);
	}
	reference erase(const_iterator first, const_iterator last) { 
		return m_children.erase(first, last); 
		return *dynamic_cast<pointer>(this);
	}
	template <class KeyType> requires std::is_convertible_v<KeyType, key_type> size_t erase(KeyType&& name)  { 
		return m_children.erase(std::forward(name)); 
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
	operator bool() const noexcept { 
		return m_children.empty(); 
	}

	template <class KeyType> iterator find(KeyType&& name) { 
		return m_children.find(std::forward<KeyType>(name)); 
	}
	template <class KeyType> const_iterator find(KeyType&& name) const { 
		return m_children.find(std::forward<KeyType>(name)); 
	}
	template <class KeyType> bool contains(KeyType&& name) const { 
		return m_children.contains(std::forward<KeyType>(name)); 
	}

	template <class KeyType> const_reference child(KeyType&& key) const {
		constexpr bool stringlike = requires(const KeyType& k) {
			key_type(key);
			key_type().find("");
			key_type().substr(0, 0);
		};

		if constexpr (stringlike) {
			key_type k(key);
			size_t beg = 0, cur;
			pointer_const p = dynamic_cast<pointer_const>(this);

			while ((cur = k.find("::", beg)) < k.size()) {
				p = p->m_children.at(k.substr(beg, cur - beg));
				(beg = cur) += 2;
			}

			return *p->m_children.at(k.substr(beg)).get();
		} else {
			return *m_children.at(key).get();
		}
	}
	template <class KeyType> reference child(KeyType&& key) {
		constexpr bool stringlike = requires(const KeyType& k) {
			key_type(key);
			key_type().find("");
			key_type().substr(0, 0);
		};

		if constexpr (stringlike) {
			key_type k(key);
			size_t beg = 0, cur;
			pointer p = dynamic_cast<pointer>(this);

			while ((cur = k.find("::", beg)) < k.size()) {
				p = p->m_children.at(k.substr(beg, cur - beg));
				beg = cur + 2;
			}

			return *p->m_children.at(k.substr(beg)).get();
		} else {
			return *m_children.at(key).get();
		}
	}

	template <class KeyType> const_reference at(KeyType&& name) const { *m_children.at(std::forward<KeyType>(name)); }
	template <class KeyType> reference at(KeyType&& name) { *m_children.at(std::forward<KeyType>(name)); }
	
	template <class KeyType> const_reference operator[](KeyType&& name) const { return *m_children[std::forward<KeyType>(name)]; }
	template <class KeyType> reference operator[](KeyType&& name) { return *m_children[std::forward<KeyType>(name)]; }

	NODISCARD size_t size() const noexcept { return m_children.size(); }
	NODISCARD key_type name() const noexcept { return m_name; }
	NODISCARD pointer parent() noexcept { return dynamic_cast<pointer>(m_parent); }
	NODISCARD const_pointer const parent() const noexcept { return dynamic_cast<pointer>(m_parent); }

protected:
	key_type m_name;

	BasicNode* m_parent = nullptr;
	container m_children;
};

template <class Ty> using Node = BasicNode<Ty, UniquePointer>;
template <class Ty> using SharedNode = BasicNode<Ty, SharedPointer>;

} // namespace lyra
