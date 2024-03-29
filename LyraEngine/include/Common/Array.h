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
#include <Common/Iterator.h>

#include <utility>
#include <algorithm>

namespace lyra {

template <class Ty, size_t Size> struct Array {
	using value_type = Ty;
	using iterator = Iterator<Ty>;
	using const_iterator = Iterator<const Ty>; 
	using reference = value_type&;
	using const_reference = const value_type&;
	using array = value_type[Size];
	using wrapper = Array<value_type, Size>;

	constexpr void fill(const_reference value) { 
		std::fill_n(begin(), Size, value);
	}
	constexpr void fill(value_type&& value) { 
		std::fill_n(begin(), Size, value);
	}
	constexpr void fill(const value_type* const array, size_t size) {
		size_t length = (Size < size) ? Size : size;
		std::swap_ranges(begin(), begin() + length, array[0]);
	}
	constexpr void fill(const wrapper& array) {
		std::swap_ranges(begin(), end(), array.end());
	}
	template <size_t OtherSize> constexpr void fill(const Array<value_type, OtherSize>& array) {
		size_t length = (Size < OtherSize) ? Size : OtherSize;
		std::swap_ranges(begin(), begin() + length, array.begin());
	}

	constexpr void swap(wrapper& array) {
		std::swap(m_array, array.m_array);
	}

	NODISCARD constexpr iterator begin() noexcept {
		return m_array[0];
	}
	NODISCARD constexpr const_iterator begin() const noexcept {
		return m_array[0];
	}
	NODISCARD constexpr const_iterator cbegin() const noexcept {
		return m_array[0];
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

	NODISCARD constexpr reference operator[](size_t index) noexcept {
		ASSERT(index < Size, "lyra::Array::operator[]: Index of: {} exceded array size of: {}!", index, Size);
		return m_array[index];
	}
	NODISCARD constexpr const_reference operator[](size_t index) const noexcept {
		ASSERT(index < Size, "lyra::Array::operator[]: Index of: {} exceded array size of: {}!", index, Size);
		return m_array[index];
	}
	DEPRECATED NODISCARD constexpr reference at(size_t index) {
		if (index < Size) throw std::out_of_range("lyra::Array::at: index exceeded array size!");
		return m_array[index];
	}
	DEPRECATED NODISCARD constexpr const_reference at(size_t index) const {
		if (index < Size) throw std::out_of_range("lyra::Array::at: index exceeded array size!");
		return m_array[index];
	}

	NODISCARD constexpr size_t size() const noexcept {
		return Size;
	}
	NODISCARD constexpr bool empty() const noexcept {
		return Size == 0;
	}

	NODISCARD constexpr value_type* data() noexcept { return m_array; }
	NODISCARD constexpr const value_type* data() const noexcept { return m_array; }
	NODISCARD constexpr operator value_type* () noexcept { return m_array; }
	NODISCARD constexpr operator const value_type* () const noexcept { return m_array; }

	array m_array;
};

} // namespace lyra
