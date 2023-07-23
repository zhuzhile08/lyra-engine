/*************************
 * @file UniquePointer.h
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

template <class Ty, class DTy = detail::DefaultDeleter<Ty>> class UniquePointer {
public:
	using value_type = Ty;
	using pointer = Ty*;
	using deleter_type = DTy;
	using wrapper = UniquePointer;

	constexpr UniquePointer() = default;
	constexpr UniquePointer(pointer pointer) : m_pointer(pointer) { }
	constexpr UniquePointer(pointer pointer, const deleter_type& deleter) : m_pointer(pointer), m_deleter(deleter) { }
	constexpr UniquePointer(pointer pointer, deleter_type&& deleter) : m_pointer(pointer), m_deleter(std::move(deleter)) { }
	constexpr UniquePointer(UniquePointer<value_type, deleter_type>&& right) : m_pointer(std::move(right.release())), m_deleter(std::move(right.deleter())) {}
	template <class P, class D> constexpr UniquePointer(UniquePointer<P, D>&& right) : m_pointer(std::move(right.release())), m_deleter(std::move(right.deleter())) {}

	~UniquePointer() noexcept {
		if (m_pointer) m_deleter(m_pointer);
	}

	UniquePointer& operator=(UniquePointer&& right) {
		assign(right.release());
		m_deleter = std::forward<deleter_type>(right.m_deleter);
		return *this;
	}
	UniquePointer<value_type, deleter_type>& operator=(pointer right) {
		assign(right);
		return *this;
	}

	template <class ... Args> NODISCARD static UniquePointer<value_type> create(Args&&... args) {
		return UniquePointer<value_type>(new value_type(std::forward<Args>(args)...));
	}

	constexpr pointer operator->() const noexcept {
		return m_pointer;
	}
	constexpr value_type& operator*() const noexcept {
		return *m_pointer;
	}

	template <class P> constexpr operator UniquePointer<P>() const noexcept {
		return UniquePointer<P>(m_pointer);
	}

	NODISCARD constexpr pointer get() const noexcept {
		return m_pointer;
	}
	NODISCARD constexpr const deleter_type& deleter() const noexcept {
		return m_deleter;
	}
	NODISCARD constexpr deleter_type& deleter() noexcept {
		return m_deleter;
	}

	NODISCARD constexpr const bool empty() const noexcept {
		return m_pointer == nullptr;
	}
	constexpr operator bool() const noexcept {
		return m_pointer != nullptr;
	}

	NODISCARD constexpr pointer release() noexcept {
		return std::exchange(m_pointer, nullptr);
	}

	constexpr void swap(UniquePointer<value_type>& second) {
		std::swap(m_pointer, second.m_pointer);
	}
	constexpr void swap(UniquePointer<value_type>&& second) {
		std::swap(m_pointer, std::move(second.m_pointer));
	}
	constexpr void swap(pointer& second) {
		std::swap(m_pointer, second);
	}
	constexpr void swap(pointer&& second) {
		std::swap(m_pointer, std::move(second));
	}

	constexpr void assign(pointer& ptr = nullptr) noexcept {
		pointer old = std::exchange(m_pointer, ptr);
		if (old) m_deleter(old);
	}
	constexpr void assign(pointer&& ptr) noexcept {
		pointer old = std::exchange(m_pointer, std::move(ptr));
		if (old) m_deleter(std::move(old));
	}

	constexpr operator pointer() const noexcept {
		return m_pointer;
	}
	template <class CTy> constexpr operator CTy() const noexcept {
		return static_cast<CTy>(m_pointer);
	}

private:
	pointer m_pointer = nullptr;
	deleter_type m_deleter;

	template <class, class>
	friend class UniquePointer;
};

} // namespace lyra
