/*************************
 * @file Array.h
 * @author zhuzhile08 (zhuzhile08@gmail.com)
 * 
 * @brief a basic array implementation
 * 
 * @date 2022-12-24
 * 
 * @copyright Copyright (c) 2022
 *************************/
#pragma once

#include <Common/Common.h>
#include <Common/Iterators.h>

#include <utility>
#include <algorithm>

namespace lyra {

template <class Ty, size_type Size> struct Array {
	static_assert(Size != 0, "lyra::Array: A zero length array is forbidden!");

	using value_type = Ty;
	using const_value = const value_type;
	using rvreference = value_type&&;
	using reference = value_type&;
	using const_reference = const_value&;
	using array = value_type[Size];
	using wrapper = Array<value_type, Size>;

	using iterator = Iterator<value_type>;
	using const_iterator = Iterator<const_value>; 
	using reverse_iterator = ReverseIterator<value_type>;
	using const_reverse_iterator = ReverseIterator<const_value>; 

	constexpr void fill(const_reference value) { 
		std::fill_n(begin(), Size, value);
	}

	constexpr void swap(wrapper& array) {
		std::swap_ranges(begin(), end(), array.begin());
	}

	NODISCARD constexpr reference front() {
		return m_array[0];
	}
	NODISCARD constexpr const_reference front() const {
		return m_array[0];
	}
	NODISCARD constexpr reference back() {
		return m_array[Size - 1];
	}
	NODISCARD constexpr const_reference back() const {
		return m_array[Size - 1];
	}

	NODISCARD constexpr iterator begin() noexcept {
		return &m_array[0];
	}
	NODISCARD constexpr const_iterator begin() const noexcept {
		return &m_array[0];
	}
	NODISCARD constexpr const_iterator cbegin() const noexcept {
		return &m_array[0];
	}
	NODISCARD constexpr iterator end() noexcept {
		return &m_array[Size];
	}
	NODISCARD constexpr const_iterator end() const noexcept {
		return &m_array[Size];
	}
	NODISCARD constexpr const_iterator cend() const noexcept {
		return &m_array[Size];
	}
	NODISCARD constexpr reverse_iterator rbegin() noexcept {
		return (&m_array[Size]) - 1;
	}
	NODISCARD constexpr const_reverse_iterator rbegin() const noexcept {
		return (&m_array[Size]) - 1;
	}
	NODISCARD constexpr const_reverse_iterator crbegin() const noexcept {
		return (&m_array[Size]) - 1;
	}
	NODISCARD constexpr reverse_iterator rend() noexcept {
		return (&m_array[0]) - 1;
	}
	NODISCARD constexpr const_reverse_iterator rend() const noexcept {
		return (&m_array[0]) - 1;
	}
	NODISCARD constexpr const_iterator crend() const noexcept {
		return (&m_array[0]) - 1;
	}

	NODISCARD constexpr reference operator[](size_type index) noexcept {
		ASSERT(index < Size, "lyra::Array::operator[]: Index of: {} exceded array size of: {}!", index, Size);
		return m_array[index];
	}
	NODISCARD constexpr const_reference operator[](size_type index) const noexcept {
		ASSERT(index < Size, "lyra::Array::operator[]: Index of: {} exceded array size of: {}!", index, Size);
		return m_array[index];
	}
	DEPRECATED NODISCARD constexpr reference at(size_type index) {
		if (index < Size) throw std::out_of_range("lyra::Array::at: index exceeded array size!");
		return m_array[index];
	}
	DEPRECATED NODISCARD constexpr const_reference at(size_type index) const {
		if (index < Size) throw std::out_of_range("lyra::Array::at: index exceeded array size!");
		return m_array[index];
	}

	NODISCARD constexpr size_type size() const noexcept {
		return Size;
	}
	NODISCARD constexpr size_type maxSize() const noexcept {
		return std::numeric_limits<size_type>::max();
	}
	NODISCARD DEPRECATED constexpr size_type max_size() const noexcept {
		return maxSize();
	}
	NODISCARD constexpr bool empty() const noexcept {
		return Size == 0;
	}

	NODISCARD constexpr value_type* data() noexcept { return m_array; }
	NODISCARD constexpr const value_type* data() const noexcept { return m_array; }

	array m_array;
};

} // namespace lyra
