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
	// using difference_type = DTy;
	using value_type = Ty;
	using pointer_type = Ty*;
	using reference_type = Ty&;
	using wrapper_type = IteratorBase;
	using wrapper_reference = wrapper_type&;
	using const_wrapper_reference = const wrapper_type&;

	/**
	 * @brief constuct a new iterator from a raw pointer
	 * 
	 * @param pointer pointer to construct from
	 */
	IteratorBase(pointer_type pointer) : m_pointer(pointer) { }
	/**
	 * @brief constuct a new iterator from a reference
	 * 
	 * @param reference reference to construct from
	 */
	IteratorBase(reference_type reference) : m_pointer(&reference) { }

	/**
	 * @brief dereference operator
	 * 
	 * @return lyra::Iterator::reference_type
	 */
	reference_type operator*() const { return *m_pointer; }
	/**
	 * @brief pointer/member of pointer access operator
	 * 
	 * @return lyra::Iterator::pointer_type
	 */
	pointer_type operator->() { return m_pointer; }
	/**
	 * @brief postfix increment operator
	 * 
	 * @return lyra::Iteraxtor& 
	 */
	wrapper_reference operator++() { 
		++m_pointer; 
		return *this; 
	}  
	/**
	 * @brief prefix increment operator
	 * 
	 * @return lyra::Iterator 
	 */
	wrapper_reference operator++(int) { 
		wrapper_type tmp = *this; 
		++(*this); 
		return tmp; 
	}
	/**
	 * @brief postfix increment operator
	 * 
	 * @return lyra::Iteraxtor& 
	 */
	wrapper_reference operator--() { 
		--m_pointer; 
		return *this; 
	}  
	/**
	 * @brief prefix increment operator
	 * 
	 * @return lyra::Iterator 
	 */
	wrapper_reference operator--(int) { 
		wrapper_type tmp = *this; 
		--(*this); 
		return tmp; 
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
	
private:   
	pointer_type m_pointer;
};

}