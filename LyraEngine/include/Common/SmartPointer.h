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

#include <Common/Common.h>

namespace lyra {

namespace detail {

template <class Ty> struct DefaultDeleter {
	using pointer = Ty*;

	constexpr DefaultDeleter() = default;
	template <class CTy> constexpr DefaultDeleter(const DefaultDeleter<CTy>& other) noexcept { }

	constexpr void operator()(pointer ptr) const noexcept {
		delete ptr;
	}
};

} // namespace detail

/**
 * @brief A smart pointer implementation
 *
 * @tparam Ty pointer type
 */
template <class Ty, class DTy = detail::DefaultDeleter<Ty>> class SmartPointer {
public:
	using value_type = Ty;
	using pointer = Ty*;
	using deleter_type = DTy;
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
	constexpr SmartPointer(pointer pointer, const deleter_type& deleter) : m_pointer(pointer), m_deleter(deleter) { }
	/**
	 * @brief construct the smart pointer
	 *
	 * @param pointer raw pointer
	 * @param deleter deleter function
	 */
	constexpr SmartPointer(pointer pointer, deleter_type&& deleter) : m_pointer(pointer), m_deleter(std::move(deleter)) { }
	/**
	 * @brief construct the smart pointer
	 *
	 * @param right pointer to copy from
	 */
	constexpr SmartPointer(SmartPointer<value_type, deleter_type>&& right) : m_pointer(std::move(right.release())), m_deleter(std::move(right.deleter())) {}
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
		m_deleter = std::forward<deleter_type>(right.m_deleter);
		return *this;
	}
	/**
	 * @brief copy the pointer from a raw pointer
	 *
	 * @param right pointer to copy from
	 *
	 * @return lyra::SmartPointer&
	 */
	SmartPointer<value_type, deleter_type>& operator=(pointer right) {
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
	template <class ... Args> NODISCARD static SmartPointer<value_type> create(Args&&... args) {
		return SmartPointer<value_type>(new value_type(std::forward<Args>(args)...));
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
	 * @return lyra::SmartPointer::value_type&
	 */
	constexpr value_type& operator*() const noexcept {
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
	 * @return lyra::SmartPointer::deleter_type& 
	 */
	NODISCARD constexpr const deleter_type& deleter() const noexcept {
		return m_deleter;
	}
	/**
	 * @brief get the deleter function
	 * 
	 * @return lyra::SmartPointer::deleter_type&
	 */
	NODISCARD constexpr deleter_type& deleter() noexcept {
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
	constexpr void swap(SmartPointer<value_type>& second) {
		std::swap(m_pointer, second.m_pointer);
	}
	/**
	 * @brief swap the internal variables of the pointer with the ones of another
	 *
	 * @param second pointer to swap with
	 */
	constexpr void swap(SmartPointer<value_type>&& second) {
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
	deleter_type m_deleter;

	template <class, class>
	friend class SmartPointer;
};

} // namespace lyra
