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

#include <Common/Common.h>
#include <Common/Array.h>
#include <Common/IteratorBase.h>

#include <type_traits>
#include <span>
#include <stdexcept>

namespace lyra {

template <typename Ty>
concept DynarrayValueType = std::is_move_assignable_v<Ty> && std::is_default_constructible_v<Ty>; // allowed dynamic array internal type

// very dangerous dynamic array implementation, please only store contents <= 4 bytes in small quantities
template <DynarrayValueType Ty, size_t Capacity> struct Dynarray {
	using value_type = Ty;
	using reference = value_type&;
	using const_reference = const value_type&;
	using array = Array<value_type, Capacity>;
	using wrapper = Dynarray<value_type, Capacity>;
	using span = std::span<value_type>;
	using const_span = std::span<const value_type>;
	using iterator = typename array::iterator;
	using const_iterator = typename array::const_iterator;

	constexpr Dynarray() noexcept = default;
	constexpr Dynarray(size_t size, const Ty& value) {
		m_size = (Capacity < size) ? Capacity : size;
		std::fill_n(begin(), m_size, value);
	}
	constexpr Dynarray(const Dynarray& other) : m_size(other.m_size), m_array(other.m_array) { }
	constexpr Dynarray(Dynarray&& other) : m_size(std::move(other.m_size)), m_array(std::move(other.m_array)) { }
	template <class Iterator> constexpr Dynarray(Iterator first, Iterator last) { 
		for (; first != last; ++first) {
			if (m_size == Capacity) break;
			push_back(*first);
		}
	}
	template <size_t OtherSize> constexpr Dynarray(const Dynarray<value_type, OtherSize>& other) { 
		m_size = (Capacity < other.m_size) ? Capacity : other.m_size;
		std::swap_ranges(begin(), begin() + m_size, other.begin());
	}
	template <size_t OtherSize> constexpr Dynarray(Dynarray<value_type, OtherSize>&& other) {
		m_size = (Capacity < other.m_size) ? Capacity : other.m_size;
		std::swap_ranges(begin(), begin() + m_size, other.begin());
	}
	constexpr Dynarray(std::initializer_list<value_type> list) {
		m_size = (Capacity < list.size()) ? Capacity : list.size();
		std::swap_ranges(begin(), begin() + m_size, list.begin());
	}

	NODISCARD constexpr reference operator[](size_t index) noexcept {
		return m_array[index];
	}
	NODISCARD constexpr const_reference operator[](size_t index) const noexcept {
		return m_array[index];
	}
	DEPRECATED NODISCARD constexpr reference at(size_t index) noexcept {
		return m_array.at(index);
	}
	DEPRECATED NODISCARD constexpr const_reference at(size_t index) const noexcept {
		return m_array.at(index);
	}

	NODISCARD constexpr reference front() {
		return m_array[0];
	}
	NODISCARD constexpr const_reference front() const {
		return m_array[0];
	}
	NODISCARD constexpr reference back() {
		return m_array[m_size - 1];
	}
	NODISCARD constexpr const_reference back() const {
		return m_array[m_size - 1];
	}
	NODISCARD constexpr iterator begin() noexcept {
		return m_array.begin();
	}
	NODISCARD constexpr const_iterator begin() const noexcept {
		return m_array.begin();
	}
	NODISCARD constexpr iterator end() noexcept {
		return &m_array[m_size];
	}
	NODISCARD constexpr const_iterator end() const noexcept {
		return &m_array[m_size];
	}

	constexpr void fill(const_reference value) { 
		m_size = Capacity;
		std::fill_n(begin(), m_size, value);
	}
	constexpr void fill(value_type&& value) { 
		m_size = Capacity;
		std::fill_n(begin(), m_size, value);
	}
	constexpr void fill(const value_type* const array, size_t size) {
		m_size = (m_size < size) ? m_size : size;
		std::swap_ranges(begin(), begin() + m_size, array[0]);
	}
	constexpr void fill(const wrapper& array) {
		m_size = array.m_size;
		std::swap_ranges(begin(), end(), array.end());
	}
	template <size_t OtherCapacity> constexpr void fill(const Dynarray<Ty, OtherCapacity>& array) {
		m_size = (Capacity < array.m_size) ? Capacity : array.m_size;
		std::swap_ranges(begin(), begin() + m_size, array.begin());
	}

	constexpr void clear() {
		m_array.fill(Ty());
		m_size = 0;
	}
	constexpr iterator insert(const_iterator pos, const_reference value) { 
		if (full()) throw std::out_of_range("lyra::Dynarray::insert: Dynamic Array is already full!");
		iterator f, b;
		for (f = end(); f >= pos; f--) {
			b = f;
			*++b = *f;
		}
		m_size++;
		*f = value;
		return f;
	}
	constexpr iterator insert(const_iterator pos, value_type&& value) {
		if (full()) throw std::out_of_range("lyra::Dynarray::insert: Dynamic Array is already full!");
		iterator f, b;
		for (f = end(); f >= pos; f--) {
			b = f;
			*++b = *f;
		}
		m_size++;
		*f = std::move(value);
		return f;
	}
	constexpr iterator insert(const_iterator begin, size_t count, const_reference value) {
		if (m_size + count > Capacity) throw std::out_of_range("lyra::Dynarray::insert: Dynamic Array is already full!");
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
	constexpr iterator insert(const_iterator begin, size_t count, value_type&& value) {
		if (m_size + count > Capacity) throw std::out_of_range("lyra::Dynarray::insert: Dynamic Array is already full!");
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
	constexpr iterator erase(const_iterator pos) {
		iterator b, pc = const_cast<value_type*>(&*pos);
		for (iterator f = pc + 1; f < end(); f++) {
			b = f;
			*--b = *f;
		}
		m_size--;
		return pc - 1;
	}
	constexpr iterator erase(const_iterator begin, const_iterator end) {
		iterator b, bc = const_cast<value_type*>(&*begin), ec = const_cast<value_type*>(&*end); 
		auto s = &*end - &*begin;
		for (iterator f = ec; f < this->end(); f++) {
			b = f;
			*(b - s) = *f;
		} 
		m_size -= s;
		return bc - 1;
	}

	constexpr void push_back(const_reference value) {
		m_array[m_size] = std::move(value);
		m_size++;
	}
	constexpr void push_back(value_type&& value) {
		m_array[m_size] = std::move(value);
		m_size++;
	}
	constexpr void pop_back() {
		back() = std::move(value_type());
		m_size--;
	}
	constexpr void resize(size_t size) {
		if (m_size <= size) m_size = size;
		else for ( ; m_size > size; m_size--) pop_back();
	}
	constexpr void swap(const wrapper array) {
		std::swap(m_array, array.m_array);
	}

	NODISCARD constexpr size_t size() const noexcept {
		return m_size;
	}
	NODISCARD constexpr size_t max_size() const noexcept {
		return Capacity;
	}
	NODISCARD constexpr bool empty() const noexcept {
		return m_size == 0;
	}
	NODISCARD constexpr bool full() const noexcept {
		return Capacity == m_size;
	}
	
	NODISCARD constexpr value_type* data() noexcept { 
		span span{ m_array.data(), m_size };
		return span.data();
	}
	NODISCARD constexpr const value_type* data() const noexcept { 
		const_span span { m_array.data(), m_size };
		return span.data();
	}
	NODISCARD constexpr value_type* all_data() noexcept { return m_array; }
	NODISCARD constexpr const value_type* all_data() const noexcept { return m_array; }
	NODISCARD constexpr operator value_type*() noexcept { return m_array; }
	NODISCARD constexpr operator const value_type* () const noexcept { return m_array; }

	array m_array;
	size_t m_size = 0;
};

} // namespace lyra
