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

/**
 * @brief a basic implementation of an iterator
 * 
 * @tparam Ty stored type
 */
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
	/**
	 * @brief constuct a new iterator from a raw pointer
	 * 
	 * @param pointer pointer to construct from
	 */
	constexpr IteratorBase(pointer pointer) noexcept : m_pointer(pointer) { }
	/**
	 * @brief constuct a new iterator from a reference
	 * 
	 * @param reference reference to construct from
	 */
	constexpr IteratorBase(reference reference) noexcept : m_pointer(&reference) { }

	/**
	 * @brief dereference operator
	 * 
	 * @return lyra::Iterator::reference
	 */
	constexpr reference operator*() { return *m_pointer; }
	/**
	 * @brief dereference operator
	 * 
	 * @return lyra::Iterator::const_reference
	 */
	constexpr const_reference operator*() const { return *m_pointer; }
	/**
	 * @brief pointer/member of pointer access operator
	 * 
	 * @return lyra::Iterator::pointer
	 */
	constexpr pointer operator->() noexcept { return m_pointer; }
	/**
	 * @brief pointer/member of pointer access operator
	 * 
	 * @return lyra::Iterator::pointer_const
	 */
	constexpr pointer_const operator->() const noexcept { return m_pointer; }
	/**
	 * @brief get the internal pointer
	 * 
	 * @return lyra::Iterator::pointer
	 */
	constexpr pointer get() noexcept { return m_pointer; }
	/**
	 * @brief get the internal pointer
	 * 
	 * @return lyra::Iterator::pointer_const
	 */
	constexpr pointer_const get() const noexcept { return m_pointer; }

	/**
	 * @brief postfix increment operator
	 * 
	 * @return lyra::IteratorBase::wrapper_reference
	 */
	constexpr wrapper_reference operator++() noexcept { 
		++m_pointer; 
		return *this; 
	}  
	/**
	 * @brief prefix increment operator
	 * 
	 * @return lyra::IteratorBase::wrapper
	 */
	constexpr wrapper operator++(int) noexcept { 
		wrapper tmp = *this; 
		++(*this); 
		return tmp; 
	}
	/**
	 * @brief postfix increment operator
	 * 
	 * @return lyra::IteratorBase::wrapper_reference
	 */
	constexpr wrapper_reference operator--() noexcept { 
		--m_pointer; 
		return *this; 
	}  
	/**
	 * @brief prefix increment operator
	 * 
	 * @return lyra::IteratorBase::wrapper
	 */
	constexpr wrapper operator--(int) noexcept { 
		wrapper tmp = *this; 
		--(*this); 
		return tmp; 
	}
	/**
	 * @brief addition operator
	 * 
	 * @param i number to add to the pointer
	 * 
	 * @return lyra::IteratorBase::wrapper
	 */
	constexpr wrapper operator+(const size_t& i) const {
		wrapper tmp = *this; 
		tmp.m_pointer += i;
		return tmp;
	}
	/**
	 * @brief subtraction operator
	 * 
	 * @param i number to add to the pointer
	 * 
	 * @return lyra::IteratorBase::wrapper
	 */
	constexpr wrapper operator-(const size_t& i) const {
		// gotta love pointer manipulation
		return *this + (-i); 
	}

	/**
	 * @brief cast to constant iterator
	 * 
	 * @return lyra::IteratorBase<lyra::IteratorBase::const_value> 
	 */
	constexpr operator IteratorBase<const_value>() const {
		return m_pointer;
	}

	/**
	 * @brief equality operator
	 * 
	 * @param other iterator to compare to
	 * 
	 * @return true if the other iterator points to the same object
	 * @return false if the other iterator doesn't point to the same object
	 */
	friend bool operator==(const_wrapper_reference first, const_wrapper_reference second) { return first.m_pointer == second.m_pointer; }
	/**
	 * @brief equality operator
	 * 
	 * @param other iterator to compare to
	 * 
	 * @return true if the other iterator doesn't point to the same object
	 * @return false if the other iterator points to the same object
	 */
	friend bool operator!=(const_wrapper_reference first, const_wrapper_reference second) { return first.m_pointer != second.m_pointer; }
	/**
	 * @brief smaller than operator
	 * 
	 * @param other iterator to compare to
	 * 
	 * @return true if the other iterator is smaller than the first
	 * @return false if the other iterator is not smaller than the first
	 */
	friend bool operator>(const_wrapper_reference first, const_wrapper_reference second) { return first.m_pointer > second.m_pointer; }
	/**
	 * @brief larger than operator
	 * 
	 * @param other iterator to compare to
	 * 
	 * @return true if the other iterator is larger than the first
	 * @return false if the other iterator is not larger than the first
	 */
	friend bool operator<(const_wrapper_reference first, const_wrapper_reference second) { return first.m_pointer < second.m_pointer; }
	/**
	 * @brief smaller or equal operator
	 * 
	 * @param other iterator to compare to
	 * 
	 * @return true if the other iterator is smaller than or equal to the first
	 * @return false if the other iterator is not smaller than or equal to the first
	 */
	friend bool operator>=(const_wrapper_reference first, const_wrapper_reference second) { return first.m_pointer >= second.m_pointer; }
	/**
	 * @brief larger or equal operator
	 * 
	 * @param other iterator to compare to
	 * 
	 * @return true if the other iterator is larger than or equal to the first
	 * @return false if the other iterator is not larger than or equal to the first
	 */
	friend bool operator<=(const_wrapper_reference first, const_wrapper_reference second) { return first.m_pointer <= second.m_pointer; }
	  
	pointer m_pointer;
};

}