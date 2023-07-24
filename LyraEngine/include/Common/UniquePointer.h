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
#include <Common/IteratorBase.h>

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
	constexpr UniquePointer(nullpointer) : m_pointer(nullptr) { }
	constexpr UniquePointer(pointer pointer) : m_pointer(pointer) { }
	template <class P> constexpr UniquePointer(P pointer) : m_pointer(pointer) { }
	constexpr UniquePointer(pointer pointer, const deleter_type& deleter) : m_pointer(pointer), m_deleter(deleter) { }
	constexpr UniquePointer(pointer pointer, deleter_type&& deleter) : m_pointer(pointer), m_deleter(std::move(deleter)) { }
	constexpr UniquePointer(UniquePointer&& right) : m_pointer(std::move(right.release())), m_deleter(std::move(right.deleter())) {}
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

// deviates from standard implementation, acts like a runtime allocated lyra::Array/std::array
template <class Ty, class DTy> class UniquePointer<Ty[], DTy> {
public:
	using value_type = Ty;
	using pointer = Ty*;
	using deleter_type = DTy;
	using wrapper = UniquePointer;

	using iterator = IteratorBase<Ty>;
	using const_iterator = IteratorBase<const Ty>; 
	using reference = value_type&;
	using const_reference = const value_type&;

	constexpr UniquePointer() = default;
	constexpr UniquePointer(nullpointer) : m_pointer(nullptr) { }
	constexpr UniquePointer(pointer pointer) : m_pointer(pointer) { }
	template <class P> constexpr UniquePointer(P pointer) : m_pointer(pointer) { }
	template <class P> constexpr UniquePointer(P pointer, const deleter_type& deleter) : m_pointer(pointer), m_deleter(deleter) { }
	template <class P> constexpr UniquePointer(P pointer, deleter_type&& deleter) : m_pointer(pointer), m_deleter(std::move(deleter)) { }
	constexpr UniquePointer(UniquePointer&& right) : m_pointer(std::move(right.release())), m_deleter(std::move(right.deleter())) {}
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

	template <class P> constexpr operator UniquePointer<P>() const noexcept {
		return UniquePointer<P>(m_pointer);
	}

	constexpr void fill(const_reference value) { 
		std::fill_n(begin(), m_size, value);
	}
	constexpr void fill(value_type&& value) { 
		std::fill_n(begin(), m_size, value);
	}
	constexpr void fill(const value_type* const array, size_t size) {
		size_t length = (m_size < size) ? m_size : size;
		std::swap_ranges(begin(), begin() + length, array[0]);
	}
	constexpr void fill(const wrapper& array) {
		size_t length = (m_size < array.m_size) ? m_size : array.m_size;
		std::swap_ranges(begin(), begin() + length, array.begin());
	}

	NODISCARD constexpr iterator begin() noexcept {
		return m_pointer[0];
	}
	NODISCARD constexpr const_iterator begin() const noexcept {
		return m_pointer[0];
	}
	NODISCARD constexpr iterator end() noexcept {
		return &m_pointer[m_size];
	}
	NODISCARD constexpr const_iterator end() const noexcept {
		return &m_pointer[m_size];
	}

	NODISCARD constexpr reference operator[](size_t index) noexcept {
		return m_pointer[index];
	}
	NODISCARD constexpr const_reference operator[](size_t index) const noexcept {
		return m_pointer[index];
	}
	DEPRECATED NODISCARD constexpr reference at(size_t index) noexcept {
		return m_pointer[index];
	}
	DEPRECATED NODISCARD constexpr const_reference at(size_t index) const noexcept {
		return m_pointer[index];
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

	NODISCARD constexpr bool empty() const noexcept {
		return m_size == 0;
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

	NODISCARD constexpr size_t size() const noexcept {
		return m_size;
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

	size_t m_size;

	template <class, class>
	friend class UniquePointer;
};

} // namespace lyra
