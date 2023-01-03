/*************************
 * @file smart_pointer.h
 * @author zhuzhile08 (zhuzhile08@gmail.com)
 * 
 * @brief A custom smart pointer implementation
 * 
 * @date 2022-10-11
 * 
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <memory>

namespace lyra {

#define NODISCARD [[nodiscard]]

/**
 * @brief A smart pointer implementation
 *
 * @tparam Ty pointer type
 */
template <class Ty, class DTy = std::default_delete<Ty>> class SmartPointer {
public:
	constexpr SmartPointer() = default;

	/**
	 * @brief construct the smart pointer
	 *
	 * @param pointer raw pointer
	 */
	constexpr SmartPointer(Ty* pointer) : m_pointer(pointer) { }
	/**
	 * @brief construct the smart pointer
	 *
	 * @param right pointer to copy from
	 */
	constexpr SmartPointer(SmartPointer<Ty, DTy>&& right) : m_pointer(right.release()), m_deleter(right.deleter) {}

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
	 * @return SmartPointer<Ty>&
	 */
	inline SmartPointer<Ty, DTy>& operator=(SmartPointer<Ty, DTy>&& right) {
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
	 * @return SmartPointer<Ty>
	 */
	template <class ... Args> NODISCARD static SmartPointer<Ty> create(Args&&... args) {
		return SmartPointer<Ty>(new Ty(std::forward<Args>(args)...));
	}

	/**
	 * @brief access the internal pointer
	 *
	 * @return Ty*
	 */
	constexpr inline Ty* operator->() const noexcept {
		return m_pointer;
	}
	/**
	 * @brief dereference the internal pointer and return the value
	 *
	 * @return Ty&
	 */
	constexpr inline Ty& operator*() const noexcept {
		return *m_pointer;
	}

	/**
	 * @brief return a smart pointer of a type which the internal pointer can cast to
	 * 
	 * @tparam _BTy castable type
	 * 
	 * @return SmartPointer<BTy> 
	 */
	template <class CTy> constexpr inline operator SmartPointer<CTy>() const noexcept {
		return SmartPointer<CTy>(m_pointer);
	}

	/**
	 * @brief get the internal raw pointer
	 *
	 * @return constexpr Ty*
	 */
	NODISCARD constexpr inline Ty* get() const noexcept {
		return m_pointer;
	}
	/**
	 * @brief get the deleter function
	 * 
	 * @return constexpr _DTy& 
	 */
	NODISCARD constexpr inline const DTy& deleter() const noexcept {
		return m_deleter;
	}
	/**
	 * @brief get the deleter function
	 * 
	 * @return constexpr _DTy&
	 */
	NODISCARD constexpr inline DTy& deleter() noexcept {
		return m_deleter;
	}

	/**
	 * @brief check if pointer is empty
	 *
	 * @return constexpr bool
	 */
	NODISCARD constexpr inline const bool empty() const noexcept {
		return m_pointer == nullptr;
	}
	/**
	 * @brief check if pointer is referencing a object
	 * 
	 * @return constexpr bool
	 */
	constexpr inline operator bool() const noexcept {
		return m_pointer != nullptr;
	}

	/**
	 * @brief release a pointer to the internal raw pointer and reset it
	 *
	 * @return constexpr Ty*
	 */
	NODISCARD constexpr inline Ty* release() noexcept {
		return std::exchange(m_pointer, nullptr);
	}
	/**
	 * @brief swap the internal variables of the pointer with the ones of another
	 *
	 * @param second pointer to swap with
	 */
	inline void swap(SmartPointer<Ty>& second) {
		std::swap(m_pointer, second.m_pointer);
	}
	/**
	 * @brief assign the internal raw pointer to another raw pointer
	 *
	 * @param ptr pointer
	 */
	inline void assign(Ty* ptr = nullptr) noexcept {
		Ty* old = std::exchange(m_pointer, ptr);
		if (old) m_deleter(old);
	}

	/**
	 * @brief cast the type to its internal pointer
	 * 
	 * @return constexpr Ty*
	 */
	constexpr inline operator Ty* () const noexcept {
		return m_pointer;
	}

	SmartPointer(const SmartPointer&) = delete;
	SmartPointer& operator=(const SmartPointer&) = delete;

private:
	Ty* m_pointer = nullptr;
	DTy m_deleter;

	template <class, class>
	friend class SmartPointer;
};

} // namespace lyra
