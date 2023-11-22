/*************************
 * @file IteratorBase.h
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

template <class Ty> class IteratorBase {
public:
	// using difference = DTy;
	using value_type = Ty;
	using const_value = const value_type;
	using pointer = Ty*;
	using const_pointer = const Ty*;
	using pointer_const = Ty* const;
	using reference = Ty&;
	using const_reference = Ty&;
	using wrapper = IteratorBase;
	using wrapper_reference = wrapper&;
	using const_wrapper_reference = const wrapper&;

	constexpr IteratorBase() noexcept = default;
	constexpr IteratorBase(pointer pointer) noexcept : m_pointer(pointer) { }
	constexpr IteratorBase(reference reference) noexcept : m_pointer(&reference) { }

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
	constexpr wrapper operator+(size_t i) const {
		wrapper tmp = *this; 
		tmp.m_pointer += i;
		return tmp;
	}
	constexpr wrapper operator-(size_t i) const {
		// gotta love pointer manipulation
		return *this + (-i); 
	}

	constexpr operator IteratorBase<const_value>() const {
		return m_pointer;
	}

	friend bool operator==(const_wrapper_reference first, const_wrapper_reference second) { return first.m_pointer == second.m_pointer; }
	friend bool operator!=(const_wrapper_reference first, const_wrapper_reference second) { return first.m_pointer != second.m_pointer; }
	friend bool operator>(const_wrapper_reference first, const_wrapper_reference second) { return first.m_pointer > second.m_pointer; }
	friend bool operator<(const_wrapper_reference first, const_wrapper_reference second) { return first.m_pointer < second.m_pointer; }
	friend bool operator>=(const_wrapper_reference first, const_wrapper_reference second) { return first.m_pointer >= second.m_pointer; }
	friend bool operator<=(const_wrapper_reference first, const_wrapper_reference second) { return first.m_pointer <= second.m_pointer; }

	pointer m_pointer;
};

}