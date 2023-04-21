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

#include <type_traits>
#include <algorithm>
#include <deque>

#include <Common/SmartPointer.h>
#include <Common/FunctionPointer.h>
#include <Common/IteratorBase.h>

#define NODISCARD [[nodiscard]]
#define DEPRECATED [[deprecated]]

namespace lyra {

/**
 * @brief A implementation of the object pool pattern, uses a std::deque as container
 * 
 * @tparam Ty type of object to store
 */
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

		/**
		 * @brief return the resource by adding it back to the pool
		 * 
		 * @param resource resource to return
		 */
		constexpr void operator() (pointer resource) {
			m_pool->emplace_back(resource);
		}
	};

	using resource_container = SmartPointer<value_type, ResourceReturner>;

	/**
	 * @brief get an element of the array
	 *
	 * @param index index of the element
	 * @return lyra::ResourcePool::reference 
	 */
	NODISCARD constexpr reference operator[](const size_t& index) noexcept {
		return *m_resources[index];
	}
	/**
	 * @brief get an element of the array
	 *
	 * @param index index of the element
	 * @return lyra::ResourcePool::const_reference 
	 */
	NODISCARD constexpr const_reference operator[](const size_t& index) const noexcept {
		return *m_resources[index];
	}
	/**
	 * @brief get an element of the array with no UB posibility
	 *
	 * @param index index of the element
	 * @return lyra::ResourcePool::reference 
	 */
	DEPRECATED NODISCARD constexpr reference at(const size_t& index) noexcept {
		return *m_resources.at(index);
	}
	/**
	 * @brief get an element of the array with no UB posibility
	 *
	 * @param index index of the element
	 * @return lyra::ResourcePool::reference 
	 */
	DEPRECATED NODISCARD constexpr const_reference at(const size_t& index) const noexcept {
		return *m_resources.at(index);
	}
	/**
	 * @brief get the front element of the array
	 *
	 * @return lyra::ResourcePool::reference
	 */
	NODISCARD constexpr reference front() {
		return *m_resources.front();
	}
	/**
	 * @brief get the front element of the array
	 *
	 * @return lyra::ResourcePool::const_reference
	 */
	NODISCARD constexpr const_reference front() const {
		return *m_resources.front();
	}
	/**
	 * @brief get the back element of the array
	 *
	 * @return lyra::ResourcePool::reference
	 */
	NODISCARD constexpr reference back() {
		return *m_resources.back();
	}
	/**
	 * @brief get the back element of the array
	 *
	 * @return lyra::ResourcePool::const_reference
	 */
	NODISCARD constexpr const_reference back() const {
		return *m_resources.back();
	}
	/**
	 * @brief get an iterator to the begin of the array
	 *
	 * @return lyra::ResourcePool::iterator
	 */
	NODISCARD constexpr iterator begin() {
		return *m_resources.begin();
	}
	/**
	 * @brief get an iterator to the begin of the array
	 *
	 * @return lyra::ResourcePool::const_iterator
	 */
	NODISCARD constexpr const_iterator begin() const {
		return *m_resources.begin();
	}
	/**
	 * @brief get an iterator to the end of the array
	 *
	 * @return lyra::ResourcePool::iterator
	 */
	NODISCARD constexpr iterator end() {
		return *m_resources.end();
	}
	/**
	 * @brief get an iterator to the begin of the array
	 *
	 * @return lyra::ResourcePool::const_iterator
	 */
	NODISCARD constexpr const_iterator end() const {
		return *m_resources.end();
	}


	// capacity

	/**
	 * @brief get the size_t of the internal deque
	 * 
	 * @return const size_t
	 */
	NODISCARD constexpr const size_t size() const noexcept {
		return m_resources.size();
	}
	/**
	 * @brief get the maximum size_t of the internal deque
	 * 
	 * @return size_t
	 */
	NODISCARD constexpr size_t max_size() const noexcept {
		return m_resources.max_size();
	}
	/**
	 * @brief check if the internal deque has elements
	 * 
	 * @return bool
	 */
	NODISCARD constexpr bool empty() const noexcept {
		return m_resources.size_t() == 0;
	}
	/**
	 * @brief return false if the internal deque is empty
	 * 
	 * @return bool
	 */
	NODISCARD constexpr operator bool() const noexcept {
		return m_resources.size_t() != 0;
	}
	/**
	 * @brief shrink the internal deque to free unused memory
	 */
	constexpr void shrink_to_fit() {
		m_resources.shrink_to_fit();
	}


	// modifiers

	/**
	 * @brief clear and destruct the smart pointers in the deque
	 */
	constexpr void clear() {
		for (auto& resource : m_resources) delete resource.release();
	}

	/**
	 * @brief insert a resource into an index
	 * 
	 * @param index index to insert nito
	 * @param value value to insert
	 * 
	 * @return lyra::ResourcePool::reference 
	 */
	reference insert(const size_t& index, const_reference value) {
		m_resources.insert(m_resources.begin() + index, value);
	}
	/**
	 * @brief insert a resource into an index
	 * 
	 * @param index index to insert into
	 * @param value value to insert
	 * 
	 * @return lyra::ResourcePool::reference 
	 */
	reference insert(const size_t& index, movable value) {
		m_resources.insert(m_resources.begin() + index, std::move(value));
	}
	/**
	 * @brief emplace an element at a specific index
	 * 
	 * @tparam Args variadic argument types
	 * 
	 * @param index index to emplace into
	 * @param args arguments to construct the resource
	 * 
	 * @return lyra::ResourcePool::reference 
	 */
	template<class... Args> reference emplace(const size_t& index, Args&&... args) {
		return m_resources.emplace(m_resources.begin() + index, resource_container::create(std::forward<Args>(args)...));
	}

	/**
	 * @brief erase resource at index
	 * 
	 * @param index index of resource to erase
	 */
	constexpr void erase(const size_t& index) {
		m_resources.erase(m_resources.begin() + index);
	}
	/**
	 * @brief erase resources from a range of resources
	 * 
	 * @param first index of first resource to erase
	 * @param last index of last resource to erase
	 */
	constexpr void erase(const size_t& first, const size_t last) {
		m_resources.erase(m_resources.begin() + first, m_resources.begin() + last);
	}

	/**
	 * @brief push an existing resource to the back of the deque
	 * 
	 * @param value object to push to the back
	 */
	constexpr void push_back(const_reference value) {
		m_resources.push_back(value);
	}
	/**
	 * @brief push an existing resource to the back of the deque
	 * 
	 * @param value object to push to the back
	 */
	constexpr void push_back(movable value) {
		m_resources.push_back(std::move(value));
	}
	/**
	 * @brief emplace an element at the back of the internal resource container
	 * 
	 * @tparam Args variadic argument type
	 * 
	 * @param args arguments for construction
	 * 
	 * @return lyra::ResourcePool::reference 
	 */
	template<class... Args> reference emplace_back(Args&&... args) {
		return *m_resources.emplace_back(smart_pointer::create(std::forward<Args>(args)...));
	}

	/**
	 * @brief pop and destruct the last resource
	 */
	constexpr void pop_back() {
		m_resources.pop_back();
	}

	/**
	 * @brief push an existing resource to the front of the deque
	 * 
	 * @param value object to push to the front
	 */
	constexpr void push_front(const_reference value) {
		m_resources.push_front(value);
	}
	/**
	 * @brief push an existing resource to the front of the deque
	 * 
	 * @param value object to push to the front
	 */
	constexpr void push_front(movable value) {
		m_resources.push_front(std::move(value));
	}
	/**
	 * @brief emplace an element at the front of the internal resource container
	 * 
	 * @tparam Args variadic argument type
	 * 
	 * @param args arguments for construction
	 * 
	 * @return lyra::ResourcePool::reference 
	 */
	template<class... Args> reference emplace_front(Args&&... args) {
		return *m_resources.emplace_front(smart_pointer::create(std::forward<Args>(args)...));
	}

	/**
	 * @brief pop and destruct the first resource
	 */
	constexpr void pop_front() {
		m_resources.pop_front();
	}

	/**
	 * @brief swap the contents of the internal resources with another deque
	 * 
	 * @param other other deque
	 */
	constexpr void swap(deque& other) noexcept {
		m_resources.swap(other);
	}
	/**
	 * @brief swap the contents of this pool the the one of another
	 * 
	 * @param other other pool
	 */
	constexpr void swap(pool& other) noexcept {
		m_resources.swap(other.m_resources);
	}


	// actuall pool implementation and functions

	resource_container get_resource() {
		resource_container resource(m_resources.front().release(), { m_resources } );
		m_resources.pop_front();
		return resource;
	};

private:
	deque m_resources;
};

} // namespace lyra
