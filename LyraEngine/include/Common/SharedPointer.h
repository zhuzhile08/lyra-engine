/*************************
 * @file SharedPointer.h
 * @author zhuzhile08 (zhuzhile08@gmail.com)
 * 
 * @brief An implementation for a shared pointer
 * 
 * @date 2023-5-10
 * 
 * @copyright Copyright (c) 2023
*************************/

#pragma once

#include <Common/Common.h>
#include <Common/UniquePointer.h>

namespace lyra {

namespace detail {

template <class Ty> class ReferenceCounter {
public:
	using value_type = Ty;
	using counter_type = uint32;

private:
	struct DeleterBase {
		using pointer = UniquePointer<DeleterBase>;
		virtual constexpr ~DeleterBase() = default;
		virtual constexpr void destroy(value_type*) = 0;
	};

	template <class D = detail::DefaultDeleter<Ty>> class Deleter : DeleterBase {
	public:
		using deleter_type = D;

		constexpr Deleter() = default;
		constexpr Deleter(const deleter_type& d) : m_d(d) { }

		constexpr void destroy(value_type* ptr) final override {
			m_d(ptr);
		}

	private:
		deleter_type m_d = deleter_type();
	};

public:
	constexpr ReferenceCounter() noexcept = default;
	template <class D> constexpr ReferenceCounter(D del) noexcept : deleter(UniquePointer<Deleter<D>>::create(std::forward<D>(del))) { }

	constexpr ReferenceCounter* increment() noexcept {
		++counter;
		return this;
	}
	constexpr ReferenceCounter* decrement() noexcept {
		--counter;
		return this;
	}

	constexpr counter_type destroy(Ty* ptr) {
		if (counter == 1) deleter->destroy(ptr);

		ptr = nullptr;
		return --counter;
	}

	counter_type counter = 1;
	DeleterBase::pointer deleter = nullptr;
};

}

template <class Ty> class SharedPointer {
public:
	using value_type = std::remove_extent_t<Ty>;
	using reference_counter_type = detail::ReferenceCounter<value_type>;
	using reference_counter = reference_counter_type*;
	using counter_type = typename reference_counter_type::counter_type;
	using pointer = Ty*;
	using wrapper = SharedPointer;

	constexpr SharedPointer() noexcept = default;
	constexpr SharedPointer(nullpointer) noexcept { }
	template <class Other> constexpr SharedPointer(Other* ptr) { 
		auto p = std::exchange(m_pointer, ptr);
		if (m_refCount) if (m_refCount->destroy(p) == 0) delete m_refCount;
		if (m_pointer) m_refCount = new reference_counter_type();
	}
	template <class Other, class Deleter> constexpr SharedPointer(Other* ptr, Deleter del) { 
		auto p = std::exchange(m_pointer, ptr);
		if (m_refCount) if (m_refCount->destroy(p) == 0) delete m_refCount;
		if (m_pointer) m_refCount = new reference_counter_type(std::forward<Deleter>(del));
	}
	constexpr SharedPointer(const wrapper& other) : m_pointer(other.m_pointer), m_refCount(other.m_refCount) { m_refCount->increment(); }
	constexpr SharedPointer(wrapper&& other) : m_pointer(std::move(other.m_pointer)), m_refCount(std::move(other.m_refCount->increment())) { }
	template <class T> constexpr SharedPointer(const SharedPointer<T>& other) : m_pointer(other.m_pointer), m_refCount(other.m_refCount) { m_refCount->increment(); }
	template <class T> constexpr SharedPointer(SharedPointer<T>&& other) : m_pointer(std::move(other.m_pointer)), m_refCount(std::move(other.m_refCount->increment())) { }
	template <class T, class D> constexpr SharedPointer(UniquePointer<T, D>&& other) { 
		auto p = std::exchange(m_pointer, std::move(other.release()));
		if (m_refCount) if (m_refCount->destroy(p) == 0) delete m_refCount;
		if (m_pointer) m_refCount = new reference_counter_type(std::move<D>(other.deleter()));
	}

	constexpr ~SharedPointer() {
		if (m_refCount) if (m_refCount->destroy(m_pointer) == 0) delete m_refCount;
	}

	constexpr SharedPointer& operator=(const wrapper& other) {
		wrapper(other).swap(*this);
		return *this;
	}
	constexpr SharedPointer& operator=(wrapper&& other) {
		wrapper(std::move(other)).swap(*this);
		return *this;
	}
	template <class T> constexpr SharedPointer& operator=(const SharedPointer<T>& other) {
		wrapper(other).swap(*this);
		return *this;
	}
	template <class T> constexpr SharedPointer& operator=(SharedPointer<T>&& other) {
		wrapper(std::move(other)).swap(*this);
		return *this;
	}
	template <class T, class D> constexpr SharedPointer& operator=(UniquePointer<T, D>&& other) {
		wrapper(std::move(other)).swap(*this);
		return *this;
	}

	template <class ... Args> NODISCARD static SharedPointer create(Args&&... args) {
		return SharedPointer(new value_type(std::forward<Args>(args)...));
	}

	void reset() {
		wrapper().swap(*this);
	}
	template <class T> void reset(T* ptr) {
		wrapper(ptr).swap(*this);
	}
	template <class T, class D> void reset(T* ptr, D del) {
		wrapper(ptr, del).swap(*this);
	}

	void swap(wrapper& other) noexcept {
		std::swap(m_pointer, other.m_pointer);
		std::swap(m_refCount, other.m_refCount);
	}

	NODISCARD constexpr pointer get() const noexcept {
		return m_pointer;
	}
	constexpr pointer operator->() const noexcept {
		return m_pointer;
	}
	constexpr value_type& operator*() const noexcept {
		return *m_pointer;
	}

	constexpr counter_type count() const noexcept {
		if (m_refCount) return m_refCount->counter;
		return 0;
	}
	DEPRECATED constexpr counter_type use_count() const noexcept {
		if (m_refCount) return m_refCount->counter;
		return 0;
	}

	constexpr operator bool() const noexcept {
		return m_pointer != nullptr;
	}
	template <class P> constexpr operator SharedPointer<P>() const noexcept {
		return SharedPointer<P>(m_pointer);
	}
	constexpr operator pointer() const noexcept {
		return m_pointer;
	}
	template <class CTy> explicit constexpr operator CTy() const noexcept {
		return static_cast<CTy>(m_pointer);
	}

private:
	pointer m_pointer = nullptr;
	reference_counter m_refCount = nullptr;

	template <class> friend class SharedPointer;
};

}
