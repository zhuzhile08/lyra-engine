/*************************
 * @file resource_pool.h
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
#include <core/smart_pointer.h>
#include <core/function_pointer.h>

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
	typedef Ty value_type;
	typedef const value_type const_value_type;
	typedef size_t size_type;
	typedef const size_type& index_type;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type&& movable;
	typedef ResourcePool<value_type> pool_type;
	typedef SmartPointer<value_type> smart_pointer_type;
	typedef std::deque<smart_pointer_type> deque_type;

	// a callable to handle returning a resource, used as deleter for the resource container
	class ResourceReturner {
		ResourceReturner(deque_type& pool) : m_pool(&pool) { };
		
		deque_type* m_pool;

		friend class ResourcePool;

	public:
		ResourceReturner() = default;

		/**
		 * @brief return the resource by adding it back to the pool
		 * 
		 * @param resource resource to return
		 */
		void operator() (pointer resource) {
			m_pool->emplace_back(resource);
		};
	};

	typedef SmartPointer<value_type, ResourceReturner> resource_container_type;


	/**
	 * @brief get an element of the array
	 *
	 * @param index index of the element
	 * @return constexpr lyra::Dynarray::reference 
	 */
	NODISCARD constexpr reference operator[](index_type index) noexcept {
		return *m_resources[index];
	}
	/**
	 * @brief get an element of the array
	 *
	 * @param index index of the element
	 * @return constexpr lyra::Dynarray::const_reference 
	 */
	NODISCARD constexpr const_reference operator[](index_type index) const noexcept {
		return *m_resources[index];
	}
	/**
	 * @brief get an element of the array with no UB posibility
	 *
	 * @param index index of the element
	 * @return constexpr lyra::Dynarray::reference 
	 */
	DEPRECATED NODISCARD constexpr reference at(index_type index) noexcept {
		return *m_resources.at(index);
	}
	/**
	 * @brief get an element of the array with no UB posibility
	 *
	 * @param index index of the element
	 * @return constexpr lyra::Dynarray::reference 
	 */
	DEPRECATED NODISCARD constexpr const_reference at(index_type index) const noexcept {
		return *m_resources.at(index);
	}
	/**
	 * @brief get an element of the array
	 *
	 * @return constexpr lyra::Dynarray::reference 
	 */
	NODISCARD constexpr reference front() {
		return *m_resources.front();
	}
	/**
	 * @brief get an element of the array
	 *
	 * @return constexpr lyra::Dynarray::const_reference 
	 */
	NODISCARD constexpr const_reference front() const {
		return *m_resources.front();
	}
	/**
	 * @brief get an element of the array
	 *
	 * @return constexpr lyra::Dynarray::reference 
	 */
	NODISCARD constexpr reference back() {
		return *m_resources.back();
	}
	/**
	 * @brief get an element of the array
	 *
	 * @return constexpr lyra::Dynarray::const_reference 
	 */
	NODISCARD constexpr const_reference back() const {
		return *m_resources.back();
	}


	// capacity

	/**
	 * @brief get the size of the internal deque
	 * 
	 * @return constexpr const size_type
	 */
	NODISCARD constexpr const size_type size() const noexcept {
		return m_resources.size();
	}
	/**
	 * @brief get the maximum size of the internal deque
	 * 
	 * @return constexpr size_type
	 */
	NODISCARD constexpr size_type max_size() const noexcept {
		return m_resources.max_size();
	}
	/**
	 * @brief check if the internal deque has elements
	 * 
	 * @return constexpr bool
	 */
	NODISCARD constexpr bool empty() const noexcept {
		return m_resources.size() == 0;
	}
	/**
	 * @brief return false if the internal deque is empty
	 * 
	 * @return constexpr bool
	 */
	NODISCARD constexpr operator bool() const noexcept {
		return m_resources.size() != 0;
	}
	/**
	 * @brief shrink the internal deque to free unused memory
	 */
	void shrink_to_fit() {
		m_resources.shrink_to_fit();
	}


	// modifiers

	/**
	 * @brief clear and destruct the smart pointers in the deque
	 */
	void clear() {
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
	reference insert(index_type index, const_reference value) {
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
	reference insert(index_type index, movable value) {
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
	template<class... Args> reference emplace(index_type index, Args&&... args) {
		return m_resources.emplace(m_resources.begin() + index, resource_container_type::create(std::forward<Args>(args)...));
	}

	/**
	 * @brief erase resource at index
	 * 
	 * @param index index of resource to erase
	 */
	void erase(index_type index) {
		m_resources.erase(m_resources.begin() + index);
	}
	/**
	 * @brief erase resources from a range of resources
	 * 
	 * @param first index of first resource to erase
	 * @param last index of last resource to erase
	 */
	void erase(index_type first, const size_type last) {
		m_resources.erase(m_resources.begin() + first, m_resources.begin() + last);
	}

	/**
	 * @brief push an existing resource to the back of the deque
	 * 
	 * @param value object to push to the back
	 */
	void push_back(const_reference value) {
		m_resources.push_back(value);
	}
	/**
	 * @brief push an existing resource to the back of the deque
	 * 
	 * @param value object to push to the back
	 */
	void push_back(movable value) {
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
		return *m_resources.emplace_back(smart_pointer_type::create(std::forward<Args>(args)...));
	}

	/**
	 * @brief pop and destruct the last resource
	 */
	void pop_back() {
		m_resources.pop_back();
	}

	/**
	 * @brief push an existing resource to the front of the deque
	 * 
	 * @param value object to push to the front
	 */
	void push_front(const_reference value) {
		m_resources.push_front(value);
	}
	/**
	 * @brief push an existing resource to the front of the deque
	 * 
	 * @param value object to push to the front
	 */
	void push_front(movable value) {
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
		return *m_resources.emplace_front(smart_pointer_type::create(std::forward<Args>(args)...));
	}

	/**
	 * @brief pop and destruct the first resource
	 */
	void pop_front() {
		m_resources.pop_front();
	}

	/**
	 * @brief swap the contents of the internal resources with another deque
	 * 
	 * @param other other deque
	 */
	void swap(deque_type& other) noexcept {
		m_resources.swap(other);
	}
	/**
	 * @brief swap the contents of this pool the the one of another
	 * 
	 * @param other other pool
	 */
	void swap(pool_type& other) noexcept {
		m_resources.swap(other.m_resources);
	}


	// actuall pool implementation and functions

	resource_container_type get_resource() {
		resource_container_type resource(m_resources.front().release(), { m_resources } );
		m_resources.pop_front();
		return resource;
	};

private:
	deque_type m_resources;
};

} // namespace lyra
