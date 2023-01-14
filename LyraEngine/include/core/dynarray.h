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

#include <core/array.h>

namespace lyra {

// very dangerous dynamic array implementation, only store contents <= 4 bytes with less than 16 capacity
template <class Ty, size_t capacity> class Dynarray {
public:
    typedef Ty value_type;
    typedef size_t size_type;
    typedef Array<value_type, capacity> array_type;
    typedef Dynarray<Ty, capacity> wrapper_type;

    /**
     * @brief copy assignment operator
     * 
     * @param right the array to copy from
     * @return wrapper_type& 
     */
    wrapper_type& operator=(const wrapper_type& right) {
        for (size_t i; i < m_size; i++) m_array[i] = right.m_array[i];
        return *this;
    }

    /**
     * @brief fill the array with the same value
     * 
     * @param value value to fill with
     */
    void fill(const value_type& value) { for (size_t i; i < m_size; i++) m_array[i] = value; }
    /**
     * @brief fill the array with the same value
     * 
     * @param value value to fill with
     */
    void fill(value_type&& value) { 
        for (size_t i; i < m_size; i++) m_array[i] = std::forward<value_type>(value); 
    }
    /**
     * @brief fill the array with elements from a C-array
     * 
     * @param array C-array
     * @param size size of the C-array
     */
    void fill(const value_type* const array, const size_t& size) {
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
    template <size_t Otherm_size> void fill(const Dynarray<Ty, Otherm_size>& array) {
        for (size_t i; i < ( m_size < array.m_size ) ? m_size : array.m_size; i++) m_array[i] = array[i];
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
     * @brief get the first element of the array
     * 
     * @return constexpr Ty&
     */
    NODISCARD constexpr Ty& begin() noexcept {
        return m_array[0];
    }
    /**
     * @brief get the first element of the array
     * 
     * @return constexpr Ty& const
     */
    NODISCARD constexpr const Ty& begin() const noexcept {
        return m_array[0];
    }
    /**
     * @brief get the last element of the array
     * 
     * @return constexpr Ty&
     */
    NODISCARD constexpr Ty& end() noexcept {
        return m_array[m_size - 1];
    }
    /**
     * @brief get the last element of the array
     * 
     * @return constexpr const Ty& 
     */
    NODISCARD constexpr const Ty& end() const noexcept {
        return m_array[m_size - 1];
    }

    /**
     * @brief get an element of the array
     * 
     * @param index index of the element
     * @return constexpr Ty&
     */
    NODISCARD constexpr Ty& operator[](const size_t& index) noexcept {
        return m_array[index];
    }
    /**
     * @brief get an element of the array
     * 
     * @param index index of the element
     * @return constexpr const Ty&
     */
    NODISCARD constexpr const Ty& operator[](const size_t& index) const noexcept {
        return m_array[index];
    }
    /**
     * @brief get an element of the array with no UB posibility
     * 
     * @param index index of the element
     * @return constexpr Ty&
     */
    NODISCARD constexpr Ty& at(const size_t& index) noexcept {
        if (index >= m_size) return m_array[m_size - 1];
        return m_array[index];
    }
    /**
     * @brief get an element of the array with no UB posibility
     * 
     * @param index index of the element
     * @return constexpr const Ty&
     */
    NODISCARD constexpr const Ty& at(const size_t& index) const noexcept {
        if (index >= m_size) return m_array[m_size - 1];
        return m_array[index];
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
     * @return constexpr value_type*
     */
    NODISCARD constexpr value_type* data() noexcept { return m_array; }
    /**
     * @brief get the raw array
     * 
     * @return constexpr const value_type*
     */
    NODISCARD constexpr const value_type* data() const noexcept { return m_array; }
    /**
     * @brief cast the wrapper to the raw array
     * 
     * @return constexpr value_type*
     */
    NODISCARD constexpr operator value_type* () noexcept { return m_array; }
    /**
     * @brief cast the wrapper to the raw array
     * 
     * @return constexpr const value_type*
     */
    NODISCARD constexpr operator const value_type* () const noexcept { return m_array; }

private:
    size_type m_size;
    array_type m_array;
};

} // namespace lyra