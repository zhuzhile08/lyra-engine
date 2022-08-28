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
#include <core/logger.h>

#include <vector>
#include <chrono>
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
 * @brief small scope-based benchmarking class
 */
class Benchmark {
public:
	// initialize and start the timer
	Benchmark() : m_start(std::chrono::high_resolution_clock::now()) { Logger::log_warning("A benchmarker was created at address: ", get_address(this)); }

	// destruct and stop the timer
	~Benchmark() {
		// calculate end time
		auto end = std::chrono::high_resolution_clock::now();

		// calculate time passed since start in miliseconds
		auto elapsed = std::chrono::time_point_cast<std::chrono::microseconds>(end).time_since_epoch().count() - std::chrono::time_point_cast<std::chrono::microseconds>(m_start).time_since_epoch().count();

		Logger::log_warning("The benchmarker at address", get_address(this), " exited its scope with a time of: ", elapsed, " microseconds!");
	}

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
};

/**
 * @brief A smart pointer implementation
 *
 * @tparam _Ty pointer type
 */
template <class _Ty, class _DTy = std::default_delete<_Ty>> class SmartPointer {
public:
	// defalut constructor
	SmartPointer() noexcept { }
	/**
	 * @brief construct the smart pointer
	 *
	 * @param pointer raw pointer
	 */
	SmartPointer(_Ty* pointer) : m_pointer(pointer) { }
	/**
	 * @brief construct the smart pointer
	 *
	 * @param right pointer to copy from
	 */
	SmartPointer(SmartPointer<_Ty, _DTy>&& right) : m_pointer(right.release()), m_deleter(right.deleter) {}

	/**
	 * @brief destructor of the pointer
	 */
	inline ~SmartPointer() noexcept {
		if (m_pointer) m_deleter(m_pointer);
	}

	/**
	 * @brief copy the pointer from a rvalue reference with deleter
	 *
	 * @param right pointer to copy from
	 *
	 * @return SmartPointer<_Ty>&
	 */
	inline SmartPointer<_Ty, _DTy>& operator=(SmartPointer<_Ty, _DTy>&& right) {
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
		return m_pointer;
	}
	/**
	 * @brief dereference the internal pointer and return the value
	 *
	 * @return _Ty&
	 */
	NODISCARD inline _Ty& operator*() const noexcept {
		return *m_pointer;
	}

	/**
	 * @brief get the internal raw pointer
	 *
	 * @return _Ty*
	 */
	NODISCARD inline _Ty* get() const noexcept {
		return m_pointer;
	}
	/**
	 * @brief get the deleter function
	 * 
	 * @return const _DTy& 
	 */
	NODISCARD inline const _DTy& deleter() const noexcept {
		return m_deleter;
	}
	/**
	 * @brief get the deleter function
	 * 
	 * _DTy&
	 */
	NODISCARD inline _DTy& deleter() noexcept {
		return m_deleter;
	}

	/**
	 * @brief check if pointer is empty
	 *
	 * @return const bool
	 */
	NODISCARD inline const bool empty() const noexcept {
		return m_pointer == nullptr;
	}
	/**
	 * @brief check if pointer is referencing a object
	 * 
	 * @return bool
	 */
	inline operator bool() const noexcept {
		return m_pointer != nullptr;
	}

	/**
	 * @brief release a pointer to the internal raw pointer and reset it
	 *
	 * @return _Ty*
	 */
	NODISCARD inline _Ty* release() noexcept {
		return std::exchange(m_pointer, nullptr);
	}
	/**
	 * @brief swap the internal variables of the pointer with the ones of another
	 *
	 * @param second pointer to swap with
	 */
	inline void swap(SmartPointer<_Ty>& second) {
		std::swap(m_pointer, second.m_pointer);
	}
	/**
	 * @brief assign the internal raw pointer to another raw pointer
	 *
	 * @param ptr pointer
	 */
	inline void assign(_Ty* ptr = nullptr) noexcept {
		_Ty* old = std::exchange(m_pointer, ptr);
		if (old) m_deleter(old);
	}

	/**
	 * @brief cast the type to its internal pointer
	 * 
	 * @return _Ty*
	 */
	inline operator _Ty* () const noexcept {
		return m_pointer;
	}

	SmartPointer(const SmartPointer&) = delete;
	SmartPointer& operator=(const SmartPointer&) = delete;

private:
	_Ty* m_pointer = nullptr;
	_DTy m_deleter;

	template <class, class>
	friend class SmartPointer;
};

} // namespace lyra