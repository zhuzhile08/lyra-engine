/**
 * @file dynarray.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief a (slightly stupid) array masked as a vector
 * 
 * @date 2023-01-14
 * 
 * @copyright Copyright (c) 2023
 */

#pragma once

#include <type_traits>

#include <core/array.h>

#define NODISCARD [[nodiscard]]

namespace lyra {

// allowed dynamic array internal type
template <typename Type>
concept DynarrayValueType = std::is_default_constructible_v<Type> && std::is_copy_assignable_v<Type> && std::is_default_constructible_v<Type>;

// very dangerous dynamic array implementation, only store contents <= 4 bytes with less than 16 capacity
template <DynarrayValueType Ty, size_t capacity> struct Dynarray {
	typedef size_t size_type;
	typedef Array<DynarrayValueType, capacity> array_type;
	typedef Dynarray<Ty, capacity> wrapper_type;

	/**
	 * @brief copy assignment operator
	 * 
	 * @param right the array to copy from
	 * 
	 * @return lyra::Dynarray::wrapper_type& 
	 */
	wrapper_type& operator=(const wrapper_type& right) {
		for (size_t i; i < m_size; i++) m_array[i] = right.m_array[i];
		return *this;
	}

	/**
	 * @brief get the first element of the array
	 *
	 * @return constexpr lyra::DynarrayValueType&
	 */
	NODISCARD constexpr DynarrayValueType& begin() noexcept {
		return m_array[0];
	}
	/**
	 * @brief get the first element of the array
	 *
	 * @return constexpr lyra::DynarrayValueType& const
	 */
	NODISCARD constexpr const DynarrayValueType& begin() const noexcept {
		return m_array[0];
	}
	/**
	 * @brief get the last element of the array
	 *
	 * @return constexpr lyra::DynarrayValueType&
	 */
	NODISCARD constexpr DynarrayValueType& end() noexcept {
		return m_array[m_size - 1];
	}
	/**
	 * @brief get the last element of the array
	 *
	 * @return constexpr const lyra::DynarrayValueType&
	 */
	NODISCARD constexpr const DynarrayValueType& end() const noexcept {
		return m_array[m_size - 1];
	}

	/**
	 * @brief get an element of the array
	 *
	 * @param index index of the element
	 * @return constexpr lyra::DynarrayValueType&
	 */
	NODISCARD constexpr DynarrayValueType& operator[](const size_t& index) noexcept {
		return m_array[index];
	}
	/**
	 * @brief get an element of the array
	 *
	 * @param index index of the element
	 * @return constexpr const lyra::DynarrayValueType&
	 */
	NODISCARD constexpr const DynarrayValueType& operator[](const size_t& index) const noexcept {
		return m_array[index];
	}
	/**
	 * @brief get an element of the array with no UB posibility
	 *
	 * @param index index of the element
	 * @return constexpr lyra::DynarrayValueType&
	 */
	NODISCARD constexpr DynarrayValueType& at(const size_t& index) noexcept {
		if (index >= m_size) return m_array[m_size - 1];
		return m_array[index];
	}
	/**
	 * @brief get an element of the array with no UB posibility
	 *
	 * @param index index of the element
	 * @return constexpr const lyra::DynarrayValueType&
	 */
	NODISCARD constexpr const DynarrayValueType& at(const size_t& index) const noexcept {
		if (index >= m_size) return m_array[m_size - 1];
		return m_array[index];
	}

	/**
	 * @brief fill the array with the same value
	 * 
	 * @param value value to fill with
	 */
	void fill(const DynarrayValueType& value) { for (size_t i; i < m_size; i++) m_array[i] = value; }
	/**
	 * @brief fill the array with the same value
	 * 
	 * @param value value to fill with
	 */
	void fill(DynarrayValueType&& value) { 
		for (size_t i; i < m_size; i++) m_array[i] = std::forward<DynarrayValueType>(value); 
	}
	/**
	 * @brief fill the array with elements from a C-array
	 * 
	 * @param array C-array
	 * @param size size of the C-array
	 */
	void fill(const DynarrayValueType* const array, const size_t& size) {
		for (size_t i; i < ( m_size < size ) ? m_size : size; i++) m_array[i] = array[i];
	}
	/**
	 * @brief fill the array with elements from another array wrapper
	 * 
	 * @param array array wrapper to fill with
	 */
	void fill(const wrapper_type& array) {
		for (size_t i; i < ( m_size < array.m_size ) ? m_size : array.m_size; i++) m_array[i] = array[i];
	}
	/**
	 * @brief fill the array with elements from a differently sized array wrapper
	 * 
	 * @tparam Otherm_size size of the array wrapper
	 * 
	 * @param array array wrapper to fill with
	 */
	template <size_t other_size> void fill(const Dynarray<Ty, other_size>& array) {
		for (size_t i; i < ( m_size < array.m_size ) ? m_size : array.m_size; i++) m_array[i] = array[i];
	}

	/**
	 * @brief clear all the members in the array and set size to 0
	 */
	void clear() {
		m_array.fill(Ty());
		m_size = 0;
	}
	/**
	 * @brief insert an element into a specified index
	 * 
	 * @param index index to insert into
	 * @param value value to insert
	 */
	void insert(const size_t& index, const DynarrayValueType& value) { 
		m_array[index] = value;
	}
	/**
	 * @brief insert an element into a specified index
	 *
	 * @param index index to insert into
	 * @param value value to insert
	 */
	void insert(const size_t& index, DynarrayValueType&& value) {
		m_array[index] = std::forward(value);
	}
	/**
	 * @brief insert an element into a part of the array
	 *
	 * @param begin starting index
	 * @param end ending index
	 * @param value value to insert
	 */
	void insert(const size_t& begin, const size_t& end, const DynarrayValueType& value) {
		for (size_t i = begin; i <= end; i++) m_array[i] = value;
	}

	/**
	 * @brief insert an element into a part of the array
	 *
	 * @param begin starting index
	 * @param end ending index
	 * @param value value to insert
	 */
	void insert(const size_t& begin, const size_t& end, DynarrayValueType&& value) {
		for (size_t i = begin; i <= end; i++) m_array[i] = std::forward(value);
	}
	/**
	 * @brief push back a value into the array
	 *
	 * @param value value to insert
	 */
	void push_back(const DynarrayValueType& value) {
		m_array[m_size] = value;
		m_size++;
	}
	/**
	 * @brief push back a value into the array
	 *
	 * @param value value to insert
	 */
	void push_back(DynarrayValueType&& value) {
		m_array[m_size] = std::forward(value);
		m_size++;
	}
	/**
	 * @brief remove the last element in the array
	 */
	void pop_back() {
		end() = Ty();
		m_size--;
	}
	/**
	 * @brief resize the array
	 * 
	 * @param size new size of the array
	 */
	void resize(const size_t& size) {
		if (m_size <= size) m_size = size;
		else for ( ; m_size > size; m_size--) pop_back();
	}
	/**
	 * @brief swap the contents of the array with another array
	 * 
	 * @param array array to swap with
	 */
	void swap(const wrapper_type array) {
		std::swap(m_array, array.m_array);
	}

	/**
	 * @brief get the size of the array
	 * 
	 * @return constexpr const size_t
	 */
	NODISCARD constexpr const size_t size() const noexcept {
		return m_size;
	}
	/**
	 * @brief get the capacity of the internal array
	 * 
	 * @return constexpr const size_t
	 */
	NODISCARD constexpr const size_t max_size() const noexcept {
		return Size;
	}
	/**
	 * @brief check if the array is sized 0
	 * 
	 * @return constexpr const bool
	 */
	NODISCARD constexpr const bool empty() const noexcept {
		return m_size == 0;
	}

	/**
	 * @brief get the raw array
	 * 
	 * @return constexpr lyra::DynarrayValueType*
	 */
	NODISCARD constexpr DynarrayValueType* data() noexcept { 
		return m_array; 
	}
	/**
	 * @brief get the raw array
	 * 
	 * @return constexpr const lyra::DynarrayValueType*
	 */
	NODISCARD constexpr const DynarrayValueType* data() const noexcept { return m_array; }
	/**
	 * @brief cast the wrapper to the raw array
	 * 
	 * @return constexpr lyra::DynarrayValueType*
	 */
	NODISCARD constexpr operator DynarrayValueType*() noexcept { return m_array; }
	/**
	 * @brief cast the wrapper to the raw array
	 * 
	 * @return constexpr const lyra::DynarrayValueType*
	 */
	NODISCARD constexpr operator const DynarrayValueType* () const noexcept { return m_array; }

	array_type m_array;
	size_type m_size = 0;
};

} // namespace lyra