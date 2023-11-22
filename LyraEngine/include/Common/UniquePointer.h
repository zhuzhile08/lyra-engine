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
	template <class CTy> constexpr DefaultDeleter(const DefaultDeleter<CTy>&) noexcept { }

	constexpr void operator()(pointer ptr) const noexcept {
		delete ptr;
	}
};

template <class Ty> struct DefaultDeleter<Ty []> {
	using pointer = Ty*;

	constexpr DefaultDeleter() = default;
	template <class CTy> constexpr DefaultDeleter(const DefaultDeleter<CTy>&) noexcept { }

	constexpr void operator()(pointer ptr) const noexcept {
		delete[] ptr;
	}
};

} // namespace detail

template <class Ty, class DTy = detail::DefaultDeleter<Ty>> class UniquePointer {
public:
	using value_type = Ty;
	using pointer = Ty*;
	using deleter_type = DTy;
	using wrapper = UniquePointer;

	constexpr UniquePointer() noexcept = default;
	constexpr UniquePointer(nullpointer) noexcept : m_pointer(nullptr) { }
	constexpr UniquePointer(pointer pointer) noexcept : m_pointer(pointer) { }
	template <class P> constexpr UniquePointer(P pointer) noexcept : m_pointer(pointer) { }
	constexpr UniquePointer(pointer pointer, const deleter_type& deleter) noexcept : m_pointer(pointer), m_deleter(deleter) { }
	constexpr UniquePointer(pointer pointer, deleter_type&& deleter) noexcept : m_pointer(pointer), m_deleter(std::move(deleter)) { }
	constexpr UniquePointer(UniquePointer&& right) noexcept : m_pointer(std::move(right.release())), m_deleter(std::move(right.deleter())) {}
	template <class P, class D> constexpr UniquePointer(UniquePointer<P, D>&& right) noexcept : m_pointer(std::move(right.release())), m_deleter(std::move(right.deleter())) {}

	~UniquePointer() noexcept {
		reset();
	}

	UniquePointer& operator=(UniquePointer&& right) {
		reset(right.release());
		m_deleter = std::forward<deleter_type>(right.m_deleter);
		return *this;
	}
	UniquePointer& operator=(pointer right) {
		reset(right);
		return *this;
	}

	template <class ... Args> NODISCARD static UniquePointer create(Args&&... args) {
		return UniquePointer(new value_type(std::forward<Args>(args)...));
	}

	constexpr pointer operator->() const noexcept {
		return m_pointer;
	}
	constexpr value_type& operator*() const noexcept {
		return *m_pointer;
	}

	NODISCARD constexpr pointer release() noexcept {
		return std::exchange(m_pointer, nullptr);
	}
	constexpr void reset(pointer ptr = nullptr) noexcept {
		pointer old = std::exchange(m_pointer, ptr);
		if (old) m_deleter(old);
	}

	constexpr void swap(UniquePointer& second) {
		std::swap(m_pointer, second.m_pointer);
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
	template <class P> constexpr operator UniquePointer<P>() const noexcept {
		return UniquePointer<P>(m_pointer);
	}
	constexpr operator pointer() const noexcept {
		return m_pointer;
	}
	template <class CTy> explicit constexpr operator CTy() const noexcept {
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

	constexpr UniquePointer() = default;
	constexpr UniquePointer(nullpointer) : m_pointer(nullptr) { }
	constexpr UniquePointer(pointer pointer) : m_pointer(pointer) { }
	template <class P> constexpr UniquePointer(P pointer) : m_pointer(pointer) { }
	template <class P> constexpr UniquePointer(P pointer, const deleter_type& deleter) : m_pointer(pointer), m_deleter(deleter) { }
	template <class P> constexpr UniquePointer(P pointer, deleter_type&& deleter) : m_pointer(pointer), m_deleter(std::move(deleter)) { }
	constexpr UniquePointer(UniquePointer&& right) : m_pointer(std::move(right.release())), m_deleter(std::move(right.deleter())) {}
	template <class P, class D> constexpr UniquePointer(UniquePointer<P, D>&& right) : m_pointer(std::move(right.release())), m_deleter(std::move(right.deleter())) {}

	~UniquePointer() noexcept {
		reset(nullptr);
	}

	UniquePointer& operator=(UniquePointer&& right) {
		reset(right.release());
		m_deleter = std::forward<deleter_type>(right.m_deleter);
		return *this;
	}
	UniquePointer& operator=(pointer right) {
		reset(right);
		return *this;
	}

	template <class ... Args> NODISCARD static UniquePointer create(size_t size) {
		return UniquePointer(new value_type[size]);
	}

	NODISCARD constexpr pointer release() noexcept {
		return std::exchange(m_pointer, nullptr);
	}
	constexpr void reset(pointer ptr = nullptr) noexcept {
		pointer old = std::exchange(m_pointer, ptr);
		if (old) m_deleter(old);
	}

	constexpr void swap(UniquePointer& second) {
		std::swap(m_pointer, second.m_pointer);
	}

	NODISCARD constexpr pointer get() const noexcept {
		return m_pointer;
	}
	NODISCARD constexpr pointer data() const noexcept {
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
	template <class P> constexpr operator UniquePointer<P[]>() const noexcept {
		return UniquePointer<P>(m_pointer);
	}
	explicit constexpr operator pointer() const noexcept {
		return m_pointer;
	}
	template <class CTy> explicit constexpr operator CTy() const noexcept {
		return static_cast<CTy>(m_pointer);
	}

private:
	pointer m_pointer = nullptr;
	deleter_type m_deleter;

	template <class, class>
	friend class UniquePointer;
};

} // namespace lyra

namespace std {

template <class Ty, class DTy> class hash<lyra::UniquePointer<Ty, DTy>> {
public:
	std::size_t operator()(const lyra::UniquePointer<Ty, DTy>& p) const {
		return hash<Ty*>()(p.get());
	}
};

}
