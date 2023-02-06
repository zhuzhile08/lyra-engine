/*************************
 * @file basic_pool.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief a basic implementation of a resource pool, STL style
 * 
 * @date 2023-02-03
 * 
 * @copyright Copyright (c) 2023
 *************************/

#pragma once

#include <type_traits>
#include <algorithm>
#include <vector>
#include <deque>

#define NODISCARD [[nodiscard]]
#define DEPRECATED [[deprecated]]

namespace lyra {

/**
 * @brief A basic pool implementation
 * 
 * @tparam Ty type of resource to store
 */
template <class Ty> class BasicPool {
public:
	typedef Ty value_type;
	typedef const Ty const_value_type;
	typedef BasicPool<Ty> pool_type;
	typedef size_t size_type;
	typedef Ty& reference;
	typedef const Ty& const_reference;
	typedef Ty* pointer;
	typedef const Ty* const_pointer;
	typedef Ty&& movable;
	typedef std::vector<Ty> vector_type;
	typedef std::vector<Ty*> pointer_vector_type;
	typedef std::deque<Ty*> pointer_deque_type;

	// intended parent of the resource type, includes some features for the pool to function properly
	class ResourceBase {
	public:
		/**
		 * @brief construct a resource base
		 * 
		 * @param pool pool the resource belongs to, cannot be changed
		 */
		ResourceBase(BasicPool& pool) : m_pool(pool) { }

	private:
		size_t m_index;
		BasicPool& m_pool;

		friend class BasicPool;
	};

	// slightly modified vector implementation
	// it's missing some functions that you also would not miss in a pool
	// (partially because I don't know how to implement them lol)

	/**
	 * @brief get an element of the array
	 *
	 * @param index index of the element
	 * @return constexpr lyra::Dynarray::reference 
	 */
	NODISCARD constexpr reference operator[](const size_type& index) noexcept {
		return m_resources[index];
	}
	/**
	 * @brief get an element of the array
	 *
	 * @param index index of the element
	 * @return constexpr lyra::Dynarray::const_reference 
	 */
	NODISCARD constexpr const_reference operator[](const size_type& index) const noexcept {
		return m_resources[index];
	}
	/**
	 * @brief get an element of the array with no UB posibility
	 *
	 * @param index index of the element
	 * @return constexpr lyra::Dynarray::reference 
	 */
	NODISCARD constexpr reference at(const size_type& index) noexcept {
		return m_resources[std::clamp(index, size_type(0), m_resources.size() - 1)];
	}
	/**
	 * @brief get an element of the array with no UB posibility
	 *
	 * @param index index of the element
	 * @return constexpr lyra::Dynarray::const_reference 
	 */
	NODISCARD constexpr const_reference at(const size_type& index) const noexcept {
		return m_resources[std::clamp(index, size_type(0), m_resources.size() - 1)];
	}

	/**
	 * @brief clear the pool, use reset instead for better name distinction
	 */
	DEPRECATED void clear() {
		m_resources.clear();
		m_unused.clear();
		m_used.clear();
	}
	/**
	 * @brief reset and clean the pool
	 */
	DEPRECATED void reset() {
		m_resources.clear();
		m_unused.clear();
		m_used.clear();
	}

	/**
	 * @brief push back a value into the pool
	 *
	 * @param value value to insert
	 */
	template<class... Args> void emplace_back(Args&&... args) {
		m_resources.emplace_back(std::forward<Args>(args)...);
		m_unused.push_back(&m_resources.back());
	}

	/**
	 * @brief push back a value into the pool
	 *
	 * @param value value to insert
	 */
	void push_back(movable value) {
		m_resources.push_back(std::forward(value));
		m_unused.push_back(&m_resources.back());
		
	}
	/**
	 * @brief push back a value into the pool
	 *
	 * @param value value to insert
	 */
	void push_back(const_reference value) {
		m_resources.push_back(value);
		m_unused.push_back(&m_resources.back());
	}

	/**
	 * @brief get the size of the resources vector
	 * 
	 * @return constexpr const size_type
	 */
	NODISCARD constexpr const size_type size() const noexcept {
		return m_resources.size();
	}
	/**
	 * @brief get the capacity of the resources vector
	 * 
	 * @return constexpr const size_type
	 */
	NODISCARD constexpr const size_type capacity() const noexcept {
		return m_resources.capacity();
	}
	/**
	 * @brief check if the resources vector has elements
	 * 
	 * @return constexpr const bool
	 */
	NODISCARD constexpr const bool empty() const noexcept {
		return m_resources.size() == 0;
	}

	/**
	 * @brief get the data of the resources vector
	 *
	 * @return constexpr lyra::Dynarray::value_type*
	 */
	NODISCARD constexpr pointer data() noexcept { 
		m_resources.data();
	}
	/**
	 * @brief get the data of the resources vector
	 *
	 * @return constexpr const lyra::Dynarray::value_type*
	 */
	NODISCARD constexpr const_pointer data() const noexcept { 
		m_resources.data();
	}
	/**
	 * @brief cast the resources vector to the raw internal array
	 * 
	 * @return constexpr lyra::Dynarray::value_type*
	 */
	NODISCARD constexpr operator pointer() noexcept { return m_resources.data(); }
	/**
	 * @brief cast the resources vector to the raw internal array
	 * 
	 * @return constexpr const lyra::Dynarray::value_type*
	 */
	NODISCARD constexpr operator const_pointer() const noexcept { return m_resources.data(); }

	// actuall pool implementation and functions

	/**
	 * @brief get a unused resource
	 * 
	 * @return constexpr reference 
	 */
	NODISCARD constexpr reference get_unused() {
		m_used.push_back(m_unused.back());
		m_unused.pop_back();
		m_used.back()->m_index = m_used.size() - 1;
		return *m_used.back();
	}
	/**
	 * @brief return a formerly used resource back to the deque to be used again
	 */
	void return_used(const_reference resource) {
		m_unused.push_back(m_used[resource.m_index]);
		m_used.erase(resource.m_index);
	}
	/**
	 * @brief get the size of the deque of the unused resources
	 * 
	 * @return constexpr const size_type 
	 */
	NODISCARD constexpr const size_type unused_size() { return m_unused.size(); }
	/**
	 * @brief get the size of the vector of the used resources
	 * 
	 * @return constexpr const size_type 
	 */
	NODISCARD constexpr const size_type used_size() { return m_used.size(); }

private:
	vector_type m_resources;

	pointer_deque_type m_unused;
	pointer_vector_type m_used;

};



} // namespace lyar
