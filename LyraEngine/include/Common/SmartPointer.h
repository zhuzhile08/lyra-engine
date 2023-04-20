/*************************
 * @file SmartPointer.h
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
	using value = Ty;
	using pointer = Ty*;
	using deleter = DTy;
	using wrapper = SmartPointer;

	constexpr SmartPointer() = default;
	/**
	 * @brief construct the smart pointer
	 *
	 * @param pointer raw pointer
	 */
	constexpr SmartPointer(pointer pointer) : m_pointer(pointer) { }
	/**
	 * @brief construct the smart pointer
	 *
	 * @param pointer raw pointer
	 * @param deleter deleter function
	 */
	constexpr SmartPointer(pointer pointer, const deleter& deleter) : m_pointer(pointer), m_deleter(deleter) { }
	/**
	 * @brief construct the smart pointer
	 *
	 * @param pointer raw pointer
	 * @param deleter deleter function
	 */
	constexpr SmartPointer(pointer pointer, deleter&& deleter) : m_pointer(pointer), m_deleter(std::move(deleter)) { }
	/**
	 * @brief construct the smart pointer
	 *
	 * @param right pointer to copy from
	 */
	constexpr SmartPointer(SmartPointer<value, deleter>&& right) : m_pointer(std::move(right.release())), m_deleter(std::move(right.deleter())) {}
	/**
	 * @brief construct the smart pointer
	 *
	 * @param right pointer to copy from
	 */
	template <class P, class D> constexpr SmartPointer(SmartPointer<P, D>&& right) : m_pointer(std::move(right.release())), m_deleter(std::move(right.deleter())) {}

	/**
	 * @brief destructor of the pointer
	 */
	~SmartPointer() noexcept {
		if (m_pointer) m_deleter(m_pointer);
	}

	/**
	 * @brief copy the pointer from another smart pointer with same deleter
	 *
	 * @param right pointer to copy from
	 *
	 * @return lyra::SmartPointer&
	 */
	SmartPointer& operator=(SmartPointer&& right) {
		assign(right.release());
		m_deleter = std::forward<deleter>(right.m_deleter);
		return *this;
	}
	/**
	 * @brief copy the pointer from a raw pointer
	 *
	 * @param right pointer to copy from
	 *
	 * @return lyra::SmartPointer&
	 */
	SmartPointer<value, deleter>& operator=(pointer right) {
		assign(right);
		return *this;
	}

	/**
	 * @brief make a smart pointer
	 *
	 * @tparam Args variadic template
	 *
	 * @param args arguments to construct the type
	 *
	 * @return lyra::SmartPointer
	 */
	template <class ... Args> NODISCARD static SmartPointer<value> create(Args&&... args) {
		return SmartPointer<value>(new value(std::forward<Args>(args)...));
	}

	/**
	 * @brief access the internal pointer
	 *
	 * @return lyra::SmartPointer::pointer
	 */
	constexpr pointer operator->() const noexcept {
		return m_pointer;
	}
	/**
	 * @brief dereference the internal pointer and return the value
	 *
	 * @return lyra::SmartPointer::value&
	 */
	constexpr value& operator*() const noexcept {
		return *m_pointer;
	}

	/**
	 * @brief return a smart pointer of a type which the internal pointer can cast to
	 * 
	 * @tparam P castable type
	 * 
	 * @return SmartPointer<P> 
	 */
	template <class P> constexpr operator SmartPointer<P>() const noexcept {
		return SmartPointer<P>(m_pointer);
	}

	/**
	 * @brief get the internal raw pointer
	 *
	 * @return lyra::SmartPointer::pointer
	 */
	NODISCARD constexpr pointer get() const noexcept {
		return m_pointer;
	}
	/**
	 * @brief get the deleter function
	 * 
	 * @return lyra::SmartPointer::deleter& 
	 */
	NODISCARD constexpr const deleter& deleter() const noexcept {
		return m_deleter;
	}
	/**
	 * @brief get the deleter function
	 * 
	 * @return lyra::SmartPointer::deleter&
	 */
	NODISCARD constexpr deleter& deleter() noexcept {
		return m_deleter;
	}

	/**
	 * @brief check if pointer is empty
	 *
	 * @return bool
	 */
	NODISCARD constexpr const bool empty() const noexcept {
		return m_pointer == nullptr;
	}
	/**
	 * @brief check if pointer is referencing a object
	 * 
	 * @return bool
	 */
	constexpr operator bool() const noexcept {
		return m_pointer != nullptr;
	}

	/**
	 * @brief release a pointer to the internal raw pointer and reset it
	 *
	 * @return lyra::SmartPointer::pointer
	 */
	NODISCARD constexpr pointer release() noexcept {
		return std::exchange(m_pointer, nullptr);
	}

	/**
	 * @brief swap the internal variables of the pointer with the ones of another
	 *
	 * @param second pointer to swap with
	 */
	constexpr void swap(SmartPointer<value>& second) {
		std::swap(m_pointer, second.m_pointer);
	}
	/**
	 * @brief swap the internal variables of the pointer with the ones of another
	 *
	 * @param second pointer to swap with
	 */
	constexpr void swap(SmartPointer<value>&& second) {
		std::swap(m_pointer, std::move(second.m_pointer));
	}
	/**
	 * @brief swap the internal variables of the pointer with the ones of another
	 *
	 * @param second pointer to swap with
	 */
	constexpr void swap(pointer& second) {
		std::swap(m_pointer, second);
	}
	/**
	 * @brief swap the internal variables of the pointer with the ones of another
	 *
	 * @param second pointer to swap with
	 */
	constexpr void swap(pointer&& second) {
		std::swap(m_pointer, std::move(second));
	}

	/**
	 * @brief assign the internal raw pointer to another raw pointer
	 *
	 * @param ptr pointer
	 */
	constexpr void assign(pointer& ptr = nullptr) noexcept {
		pointer old = std::exchange(m_pointer, ptr);
		if (old) m_deleter(old);
	}
	/**
	 * @brief assign the internal raw pointer to another raw pointer
	 *
	 * @param ptr pointer
	 */
	constexpr void assign(pointer&& ptr) noexcept {
		pointer old = std::exchange(m_pointer, std::move(ptr));
		if (old) m_deleter(std::move(old));
	}

	/**
	 * @brief cast the type to its internal pointer
	 * 
	 * @return lyra::SmartPointer::pointer
	 */
	constexpr operator pointer() const noexcept {
		return m_pointer;
	}
	/**
	 * @brief cast the type to it a type castable from the internal pointer typo
	 * 
	 * @tparam CTy castable type
	 * 
	 * @return CTy
	 */
	template <class CTy> constexpr operator CTy() const noexcept {
		return static_cast<CTy>(m_pointer);
	}

private:
	pointer m_pointer = nullptr;
	deleter m_deleter;

	template <class, class>
	friend class SmartPointer;
};

} // namespace lyra