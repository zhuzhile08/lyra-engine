/*************************
 * @file Iterator.h
 * @author zhuzhile08 (zhuzhile08@gmail.com)
 * 
 * @brief a iterator base class
 * 
 * @date 2023-03-06
 * 
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <cstddef>
#include <iterator>

namespace lyra {

template <class Ty> class Iterator {
public:
	// using difference = DTy;
	using value_type = Ty;
	using const_value = const value_type;
	using pointer = Ty*;
	using const_pointer = const Ty*;
	using pointer_const = Ty* const;
	using reference = Ty&;
	using const_reference = Ty&;
	using wrapper = Iterator;
	using wrapper_reference = wrapper&;
	using const_wrapper_reference = const wrapper&;

	constexpr Iterator() noexcept = default;
	constexpr Iterator(pointer pointer) noexcept : m_pointer(pointer) { }
	constexpr Iterator(reference reference) noexcept : m_pointer(&reference) { }

	constexpr reference operator*() { return *m_pointer; }
	constexpr const_reference operator*() const { return *m_pointer; }
	constexpr pointer operator->() noexcept { return m_pointer; }
	constexpr pointer_const operator->() const noexcept { return m_pointer; }
	constexpr pointer get() noexcept { return m_pointer; }
	constexpr pointer_const get() const noexcept { return m_pointer; }

	constexpr wrapper_reference operator++() noexcept { 
		++m_pointer; 
		return *this; 
	}
	constexpr wrapper operator++(int) noexcept { 
		wrapper tmp = *this; 
		++(*this); 
		return tmp; 
	}
	constexpr wrapper_reference operator--() noexcept { 
		--m_pointer; 
		return *this; 
	}
	constexpr wrapper operator--(int) noexcept { 
		wrapper tmp = *this; 
		--(*this); 
		return tmp; 
	}

	friend constexpr wrapper operator+(const wrapper& it, size_t i) noexcept {
		return wrapper(it.m_pointer + i);
	}
	friend constexpr wrapper operator-(const wrapper& it, size_t i) noexcept {
		return it + (-i); 
	}
	friend constexpr wrapper operator+(size_t i, const wrapper& it) noexcept {
		return wrapper(it.m_pointer + i);
	}
	friend constexpr wrapper operator-(size_t i, const wrapper& it) noexcept {
		return it + (-i); 
	}
	friend constexpr size_t operator+(const wrapper& first, const wrapper& second) noexcept {
		return wrapper(first.m_pointer + second.m_pointer);
	}
	friend constexpr size_t operator-(const wrapper& first, const wrapper& second) noexcept {
		return first.m_pointer - second.m_pointer;
	}

	constexpr operator Iterator<const_value>() const noexcept {
		return m_pointer;
	}

	friend constexpr bool operator==(const_wrapper_reference first, const_wrapper_reference second) noexcept { return first.m_pointer == second.m_pointer; }
	friend constexpr bool operator!=(const_wrapper_reference first, const_wrapper_reference second) noexcept { return first.m_pointer != second.m_pointer; }
	friend constexpr bool operator>(const_wrapper_reference first, const_wrapper_reference second) noexcept { return first.m_pointer > second.m_pointer; }
	friend constexpr bool operator<(const_wrapper_reference first, const_wrapper_reference second) noexcept { return first.m_pointer < second.m_pointer; }
	friend constexpr bool operator>=(const_wrapper_reference first, const_wrapper_reference second) noexcept { return first.m_pointer >= second.m_pointer; }
	friend constexpr bool operator<=(const_wrapper_reference first, const_wrapper_reference second) noexcept { return first.m_pointer <= second.m_pointer; }

	pointer m_pointer;
};

}