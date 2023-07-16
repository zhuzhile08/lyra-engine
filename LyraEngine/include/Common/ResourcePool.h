/*************************
 * @file ResourcePool.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief a basic implementation of a object pool, STL style
 * 
 * @date 2023-02-03
 * 
 * @copyright Copyright (c) 2023
 *************************/

#pragma once

#include <Common/Common.h>

#include <type_traits>
#include <algorithm>
#include <deque>

#include <Common/SmartPointer.h>
#include <Common/FunctionPointer.h>
#include <Common/IteratorBase.h>

namespace lyra {

template <class Ty> class ResourcePool {
public:
	using value_type = Ty;
	using const_value = const value_type;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using movable = value_type&&;
	using pool = ResourcePool<value_type>;
	using smart_pointer = SmartPointer<value_type>;
	using deque = std::deque<smart_pointer>;
	using iterator = typename deque::iterator;
	using const_iterator = typename deque::const_iterator;

	// a callable to handle returning a resource, used as deleter for the resource container
	class ResourceReturner {
		ResourceReturner(deque& pool) : m_pool(&pool) { };
		
		deque* m_pool;

		friend class ResourcePool;

	public:
		ResourceReturner() = default;

		constexpr void operator() (pointer resource) {
			m_pool->emplace_back(resource);
		}
	};

	using resource_container = SmartPointer<value_type, ResourceReturner>;

	NODISCARD constexpr reference operator[](size_t index) noexcept {
		return *m_resources[index];
	}
	NODISCARD constexpr const_reference operator[](size_t index) const noexcept {
		return *m_resources[index];
	}
	NODISCARD constexpr reference at(size_t index) noexcept {
		return *m_resources.at(index);
	}
	NODISCARD constexpr const_reference at(size_t index) const noexcept {
		return *m_resources.at(index);
	}
	NODISCARD constexpr reference front() {
		return *m_resources.front();
	}
	NODISCARD constexpr const_reference front() const {
		return *m_resources.front();
	}
	NODISCARD constexpr reference back() {
		return *m_resources.back();
	}
	NODISCARD constexpr const_reference back() const {
		return *m_resources.back();
	}
	NODISCARD constexpr iterator begin() {
		return *m_resources.begin();
	}
	NODISCARD constexpr const_iterator begin() const {
		return *m_resources.begin();
	}
	NODISCARD constexpr iterator end() {
		return *m_resources.end();
	}
	NODISCARD constexpr const_iterator end() const {
		return *m_resources.end();
	}

	NODISCARD constexpr size_t size() const noexcept {
		return m_resources.size();
	}
	NODISCARD constexpr size_t max_size() const noexcept {
		return m_resources.max_size();
	}
	NODISCARD constexpr bool empty() const noexcept {
		return m_resources.size() == 0;
	}
	NODISCARD constexpr operator bool() const noexcept {
		return m_resources.size() != 0;
	}
	constexpr void shrink_to_fit() {
		m_resources.shrink_to_fit();
	}

	constexpr void clear() {
		for (auto& resource : m_resources) delete resource.release();
	}

	reference insert(size_t index, const_reference value) {
		m_resources.insert(m_resources.begin() + index, value);
	}
	reference insert(size_t index, movable value) {
		m_resources.insert(m_resources.begin() + index, std::move(value));
	}
	template<class... Args> reference emplace(size_t index, Args&&.. s. args) {
		return m_resources.emplace(m_resources.begin() + index, resource_container::create(std::forward<Args>(args)...));
	}

	constexpr void erase(size_t index) {
		m_resources.erase(m_resources.begin() + index);
	}
	constexpr void erase(size_t first, size_t last) {
		m_resources.erase(m_resources.begin() + first, m_resources.begin() + last);
	}

	constexpr void push_front(const_reference value) {
		m_resources.push_front(value);
	}
	constexpr void push_front(movable value) {
		m_resources.push_front(std::move(value));
	}
	constexpr void push_back(const_reference value) {
		m_resources.push_back(value);
	}
	constexpr void push_back(movable value) {
		m_resources.push_back(std::move(value));
	}
	template<class... Args> reference emplace_back(Args&&... args) {
		return *m_resources.emplace_back(smart_pointer::create(std::forward<Args>(args)...));
	}
	template<class... Args> reference emplace_front(Args&&... args) {
		return *m_resources.emplace_front(smart_pointer::create(std::forward<Args>(args)...));
	}
	constexpr void pop_back() {
		m_resources.pop_back();
	}
	constexpr void pop_front() {
		m_resources.pop_front();
	}

	constexpr void swap(deque& other) noexcept {
		m_resources.swap(other);
	}
	constexpr void swap(pool& other) noexcept {
		m_resources.swap(other.m_resources);
	}

	resource_container get_resource() {
		resource_container resource(m_resources.front().release(), { m_resources } );
		m_resources.pop_front();
		return resource;
	};

private:
	deque m_resources;
};

} // namespace lyra
