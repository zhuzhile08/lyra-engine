/************************
 * @file Dynarray.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief a (slightly stupid) array masked as a vector
 * 
 * @date 2023-01-14
 * 
 * @copyright Copyright (c) 2023
 ************************/

#pragma once

#include <type_traits>
#include <span>
#include <stdexcept>

#include <Common/Array.h>
#include <Common/IteratorBase.h>

#define NODISCARD [[nodiscard]]
#define DEPRECATED [[deprecated]]

namespace lyra {

// allowed dynamic array internal type
template <typename Ty>
concept DynarrayValueType = std::is_move_assignable_v<Ty> && std::is_default_constructible_v<Ty>;

// very dangerous dynamic array implementation, please only store contents <= 4 bytes in small quantities capacity
template <DynarrayValueType Ty, size_t capacity> struct Dynarray {
	using value = Ty;
	using reference = value&;
	using const_reference = const value&;
	using array = Array<value, capacity>;
	using wrapper = Dynarray<value, capacity>;
	using span = std::span<value>;
	using const_span = std::span<const value>;
	using iterator = typename array::iterator;
	using const_iterator = typename array::const_iterator;

	/**
	 * @brief get an element of the dynamic array
	 *
	 * @param index index of the element
	 * @return lyra::Dynarray::iterator
	 */
	NODISCARD constexpr reference operator[](const size_t& index) noexcept {
		return m_array[index];
	}
	/**
	 * @brief get an element of the dynamic array
	 *
	 * @param index index of the element
	 * @return lyra::Dynarray::const_reference 
	 */
	NODISCARD constexpr const_reference operator[](const size_t& index) const noexcept {
		return m_array[index];
	}
	/**
	 * @brief get an element of the dynamic array with no UB posibility
	 *
	 * @param index index of the element
	 * @return lyra::Dynarray::reference 
	 */
	DEPRECATED NODISCARD constexpr reference at(const size_t& index) noexcept {
		return m_array[index];
	}
	/**
	 * @brief get an element of the dynamic array with no UB posibility
	 *
	 * @param index index of the element
	 * @return lyra::Dynarray::const_reference 
	 */
	DEPRECATED NODISCARD constexpr const_reference at(const size_t& index) const noexcept {
		return m_array[index];
	}
	/**
	 * @brief get the front element of the dynamic array
	 *
	 * @return lyra::Dynarray::reference
	 */
	NODISCARD constexpr reference front() {
		return m_array[0];
	}
	/**
	 * @brief get the front element of the dynamic array
	 *
	 * @return lyra::Dynarray::const_reference
	 */
	NODISCARD constexpr const_reference front() const {
		return m_array[0];
	}
	/**
	 * @brief get the back element of the dynamic array
	 *
	 * @return lyra::Dynarray::reference
	 */
	NODISCARD constexpr reference back() {
		return m_array[m_size - 1];
	}
	/**
	 * @brief get the back element of the dynamic array
	 *
	 * @return lyra::Dynarray::const_reference
	 */
	NODISCARD constexpr const_reference back() const {
		return m_array[m_size - 1];
	}
	/**
	 * @brief get the first element of the dynamic array
	 *
	 * @return lyra::Dynarray::iterator
	 */
	NODISCARD constexpr iterator begin() noexcept {
		return m_array[0];
	}
	/**
	 * @brief get the first element of the dynamic array
	 *
	 * @return lyra::Dynarray::const_iterator
	 */
	NODISCARD constexpr const_iterator begin() const noexcept {
		return m_array[0];
	}
	/**
	 * @brief get the last element of the dynamic array
	 *
	 * @return lyra::Dynarray::iterator
	 */
	NODISCARD constexpr iterator end() noexcept {
		return &m_array[m_size];
	}
	/**
	 * @brief get the last element of the dynamic array
	 *
	 * @return lyra::Dynarray::const_iterator
	 */
	NODISCARD constexpr const_iterator end() const noexcept {
		return &m_array[m_size];
	}

	/**
	 * @brief fill the dynamic array with the same value
	 * 
	 * @param value value to fill with
	 */
	constexpr void fill(const_reference value) { for (size_t i; i < m_size; i++) m_array[i] = std::move(value); }
	/**
	 * @brief fill the dynamic array with the same value
	 * 
	 * @param value value to fill with
	 */
	constexpr void fill(value&& value) { 
		for (size_t i = 0; i < m_size; i++) m_array[i] = std::move(value); 
	}
	/**
	 * @brief fill the dynamic array with elements from a C-array
	 * 
	 * @param array C-array
	 * @param size size of the C-array
	 */
	constexpr void fill(const value* const array, const size_t& size) {
		for (size_t i = 0; i < ( m_size < size ) ? m_size : size; i++) m_array[i] = std::move(array[i]);
	}
	/**
	 * @brief fill the dynamic array with elements from another array wrapper
	 * 
	 * @param array array wrapper to fill with
	 */
	constexpr void fill(const wrapper& array) {
		for (size_t i = 0; i < ( m_size < array.m_size ) ? m_size : array.m_size; i++) m_array[i] = std::move(array[i]);
	}
	/**
	 * @brief fill the dynamic array with elements from a differently sized array wrapper
	 * 
	 * @tparam Otherm_size size of the dynamic array wrapper
	 * 
	 * @param array array wrapper to fill with
	 */
	template <size_t other_size> constexpr void fill(const Dynarray<Ty, other_size>& array) {
		for (size_t i = 0; i < ( m_size < array.m_size ) ? m_size : array.m_size; i++) m_array[i] = std::move(array[i]);
	}

	/**
	 * @brief clear all the members in the dynamic array and set size to 0
	 */
	constexpr void clear() {
		m_array.fill(Ty());
		m_size = 0;
	}
	/**
	 * @brief insert an element in front of a specified position
	 * 
	 * @param pos position to insert in front to
	 * @param value value to insert
	 * 
	 * @return lyra::Dynarray::iterator
	 */
	constexpr iterator insert(const_iterator pos, const_reference value) { 
		if (full()) throw std::out_of_range("lyra::Dynarray::insert: Dynamic Array already full!");
		iterator f, b;
		for (f = end(); f >= pos; f--) {
			b = f;
			*++b = *f;
		}
		m_size++;
		*f = value;
		return f;
	}
	/**
	 * @brief insert an element in front of a specified position
	 * 
	 * @param index position to insert in front to
	 * @param value value to insert
	 * 
	 * @return lyra::Dynarray::iterator
	 */
	constexpr iterator insert(const_iterator pos, value&& value) {
		if (full()) throw std::out_of_range("lyra::Dynarray::insert: Dynamic Array already full!");
		iterator f, b;
		for (f = end(); f >= pos; f--) {
			b = f;
			*++b = *f;
		}
		m_size++;
		*f = std::move(value);
		return f;
	}
	/**
	 * @brief insert a number of elements into the dynamic array
	 *
	 * @param begin starting iterator
	 * @param count number of elements to insert
	 * @param value value to insert
	 * 
	 * @return lyra::Dynarray::iterator
	 */
	constexpr iterator insert(const_iterator begin, const size_t& count, const_reference value) {
		if (m_size + count > capacity) throw std::out_of_range("lyra::Dynarray::insert: Dynamic Array already full!");
		iterator f, b;
		for (f = this->end(); f >= begin; f--) {
			b = f;
			*(++b + count) = *f;
		} for (size_t i = 0; i < count; i++) {
			*(f + i) = value;
			m_size++;
		}
		return f;
	}
	/**
	 * @brief insert a number of elements into the dynamic array
	 *
	 * @param begin starting iterator
	 * @param count number of elements to insert
	 * @param value value to insert
	 * 
	 * @return lyra::Dynarray::iterator
	 */
	constexpr iterator insert(const_iterator begin, const size_t& count, value&& value) {
		if (m_size + count > capacity) throw std::out_of_range("lyra::Dynarray::insert: Dynamic Array already full!");
		iterator f, b;
		for (f = this->end(); f >= begin; f--) {
			b = f;
			*(++b + count) = *f;
		} for (size_t i = 0; i < count; i++) {
			*(f + i) = std::move(value);
			m_size++;
		}
		return f;
	}
	/**
	 * @brief erase an element at a specific position and return the iterator in front of it
	 * 
	 * @param pos position to erase
	 * 
	 * @return lyra::Dynarray::iterator
	 */
	constexpr iterator erase(const_iterator pos) {
		iterator b, pc = const_cast<value*>(&*pos);
		for (iterator f = pc + 1; f < end(); f++) {
			b = f;
			*--b = *f;
		}
		m_size--;
		return pc - 1;
	}
	/**
	 * @brief erase a number of elements and return the iterator in front of it
	 * 
	 * @param pos position to erase
	 * 
	 * @return lyra::Dynarray::iterator
	 */
	constexpr iterator erase(const_iterator begin, const_iterator end) {
		iterator b, bc = const_cast<value*>(&*begin), ec = const_cast<value*>(&*end); 
		for (iterator f = ec; f < this->end(); f++) {
			b = f;
			*(b - (&*end - &*begin)) = *f;
		} 
		m_size -= (&*end - &*begin);
		return bc - 1;
	}

	/**
	 * @brief push back a value into the dynamic array
	 *
	 * @param value value to insert
	 */
	constexpr void push_back(const_reference value) {
		m_array[m_size] = std::move(value);
		m_size++;
	}
	/**
	 * @brief push back a value into the dynamic array
	 *
	 * @param value value to insert
	 */
	constexpr void push_back(value&& value) {
		m_array[m_size] = std::move(value);
		m_size++;
	}
	/**
	 * @brief remove the last element in the dynamic array
	 */
	constexpr void pop_back() {
		back() = std::move(value());
		m_size--;
	}
	/**
	 * @brief resize the dynamic array
	 * 
	 * @param size new size of the dynamic array
	 */
	constexpr void resize(const size_t& size) {
		if (m_size <= size) m_size = size;
		else for ( ; m_size > size; m_size--) pop_back();
	}
	/**
	 * @brief swap the contents of the dynamic array with another array
	 * 
	 * @param array array to swap with
	 */
	constexpr void swap(const wrapper array) {
		std::swap(m_array, array.m_array);
	}

	/**
	 * @brief get the size of the dynamic array
	 * 
	 * @return size_t
	 */
	NODISCARD constexpr size_t size() const noexcept {
		return m_size;
	}
	/**
	 * @brief get the capacity of the internal array
	 * 
	 * @return size_t
	 */
	NODISCARD constexpr size_t max_size() const noexcept {
		return capacity;
	}
	/**
	 * @brief check if the dynamic array is sized 0
	 * 
	 * @return bool
	 */
	NODISCARD constexpr bool empty() const noexcept {
		return m_size == 0;
	}
	/**
	 * @brief check if the dynamic array is already full
	 * 
	 * @return bool
	 */
	NODISCARD constexpr bool full() const noexcept {
		return capacity == m_size;
	}
	

	/**
	 * @brief get the used part of the dynamic array
	 *
	 * @return lyra::Dynarray::value*
	 */
	NODISCARD constexpr value* data() noexcept { 
		span span{ m_array.data(), m_size };
		return span.data();
	}
	/**
	 * @brief get the used part of the dynamic array
	 *
	 * @return const lyra::Dynarray::value*
	 */
	NODISCARD constexpr const value* data() const noexcept { 
		const_span span { m_array.data(), m_size };
		return span.data();
	}
	/**
	 * @brief get the raw array
	 * 
	 * @return lyra::Dynarray::value*
	 */
	NODISCARD constexpr value* all_data() noexcept { 
		return m_array; 
	}
	/**
	 * @brief get the raw array
	 * 
	 * @return const lyra::Dynarray::value*
	 */
	NODISCARD constexpr const value* all_data() const noexcept { return m_array; }
	/**
	 * @brief cast the wrapper to the raw array
	 * 
	 * @return lyra::Dynarray::value*
	 */
	NODISCARD constexpr operator value*() noexcept { return m_array; }
	/**
	 * @brief cast the wrapper to the raw array
	 * 
	 * @return const lyra::Dynarray::value*
	 */
	NODISCARD constexpr operator const value* () const noexcept { return m_array; }

	array m_array;
	size_t m_size = 0;
};

} // namespace lyra
