/*************************
 * @file util.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief miscelanious functions
 * 
 * @date 2022-02-03
 * 
 * @copyright Copyright (c) 2022
 * 
 *************************/

#pragma once

#include <core/decl.h>

#include <vector>
#include <utility>

namespace lyra {

/**
 * @brief get the size of an array
 * 
 * @tparam _Ty type of array
 * @param array the array
 * 
 * @return uint32_t
*/
template<typename _Ty> NODISCARD inline uint32 arr_size(const _Ty* array); 

template<typename _Ty> uint32 arr_size(const _Ty* array) {
	return sizeof(array) / sizeof(array[0]);
}

/**
 * @brief get the address of a pointer
 * 
 * @tparam _Ty type of pointer
 * @param type the pointer
 * 
 * @return std::string 
*/
template<typename _Ty> NODISCARD inline const void* get_address(const _Ty type);

template<typename _Ty> const void* get_address(const _Ty type) {
	return static_cast<const void*>(type);
}

/**
 * @brief move a element of a vector to another
 * 
 * @tparam _Ty type of vector
 * @param src source vector
 * @param dst destination vector
 * @param index index of the element
 */
template<typename _Ty> void move_element(std::vector<_Ty>& src, std::vector<_Ty>& dst, int index);

template<typename _Ty> void move_element(std::vector<_Ty>& src, std::vector<_Ty>& dst, int index) {
	dst.push_back(std::move(src.at(index)));
	src.erase(src.begin() + index);
}

/**
 * @brief A smart pointer implementation
 *
 * @tparam _Ty pointer type
 */
template <class _Ty> class SmartPointer {
public:
    // defalut constructor
    SmartPointer() noexcept { }
    /**
     * @brief construct the smart pointer
     *
     * @param pointer raw pointer
     */
    SmartPointer(_Ty* pointer) : _pointer(pointer) { }
    /**
     * @brief construct the smart pointer
     *
     * @param right pointer to copy from
     */
    SmartPointer(SmartPointer<_Ty>&& right) : _pointer(right.release()), _deleter(right.deleter) {}

    /**
     * @brief destructor of the pointer
     */
    inline ~SmartPointer() noexcept {
        if (_pointer) delete _pointer;
    }

    /**
     * @brief copy the pointer from a rvalue reference
     *
     * @param right pointer to copy from
     *
     * @return SmartPointer<_Ty>&
     */
    inline SmartPointer<_Ty>& operator=(SmartPointer<_Ty>&& right) {
        assign(right.release());
        return *this;
    }

    /**
     * @brief make a smart pointer
     *
     * @tparam Args variadic template
     *
     * @param args arguments to construct the type
     *
     * @return SmartPointer<_Ty>
     */
    template <class ... Args> NODISCARD static SmartPointer<_Ty> create(Args&&... args) {
        return SmartPointer<_Ty>(new _Ty(std::forward<Args>(args)...));
    }

    /**
     * @brief access the internal pointer
     *
     * @return _Ty*
     */
    NODISCARD inline _Ty* operator->() const noexcept {
        return _pointer;
    }
    /**
     * @brief dereference the internal pointer and return the value
     *
     * @return _Ty&
     */
    NODISCARD inline _Ty& operator*() const noexcept {
        return *_pointer;
    }
    /**
     * @brief get the internal raw pointer
     *
     * @return _Ty*
     */
    NODISCARD inline _Ty* get() const noexcept {
        return _pointer;
    }

    /**
     * @brief check if pointer is empty
     *
     * @return const bool
     */
    NODISCARD inline const bool empty() const noexcept {
        return _pointer == nullptr;
    }
    /**
     * @brief check if pointer is referencing a object
     * 
     * @return bool
     */
    inline operator bool() const noexcept {
        return _pointer != nullptr;
    }

    /**
     * @brief release a pointer to the internal raw pointer and reset it
     *
     * @return _Ty*
     */
    NODISCARD inline _Ty* release() noexcept {
        return std::exchange(_pointer, nullptr);
    }
    /**
     * @brief swap the internal variables of the pointer with the ones of another
     *
     * @param second pointer to swap with
     */
    inline void swap(SmartPointer<_Ty>& second) {
        std::swap(_pointer, second._pointer);
    }
    /**
     * @brief assign the internal raw pointer to another raw pointer
     *
     * @param ptr pointer
     */
    inline void assign(_Ty* ptr = nullptr) noexcept {
        _Ty* old = std::exchange(_pointer, ptr);
        if (old) delete old;
    }

    /**
     * @brief cast the type to its internal pointer
     * 
     * @return _Ty*
     */
    inline operator _Ty* () const noexcept {
        return _pointer;
    }

    SmartPointer(const SmartPointer&) = delete;
    SmartPointer& operator=(const SmartPointer&) = delete;

private:
    _Ty* _pointer = nullptr;

    template <class>
    friend class SmartPointer;
};

} // namespace lyra