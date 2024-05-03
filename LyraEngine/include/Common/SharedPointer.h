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

template <class Ty> class SharedPointer {
private:
	class ReferenceCounter {
	public:
		using value_type = Ty;
		using counter_type = uint32;

	private:
		struct DeleterBase {
			virtual constexpr ~DeleterBase() = default;
			virtual constexpr void destroy(value_type*) = 0;
		};

		template <class D = detail::DefaultDeleter<value_type>> class Deleter : public DeleterBase {
		public:
			using deleter_type = D;

			constexpr Deleter() = default;
			constexpr Deleter(const deleter_type& d) : m_d(d) { }

			constexpr void destroy(value_type* ptr) override {
				m_d(ptr);
			}

		private:
			NO_UNIQUE_ADDRESS deleter_type m_d = deleter_type();
		};

		using deleter_type = UniquePointer<DeleterBase>;

	public:
		constexpr ReferenceCounter() noexcept : m_deleter(UniquePointer<Deleter<>>::create()) { }
		template <class D> constexpr ReferenceCounter(D del) noexcept : m_deleter(UniquePointer<Deleter<D>>::create(std::forward<D>(del))) { }
		constexpr ReferenceCounter(ReferenceCounter&& other) : m_counter(other.m_counter), m_deleter(std::move(other.m_deleter)) { }
		template <class OTy> constexpr ReferenceCounter(SharedPointer<OTy>::ReferenceCounter&& other) : m_counter(other.m_counter), m_deleter(std::move(other.m_deleter)) { }

		constexpr ReferenceCounter* increment() noexcept {
			++m_counter;
			return this;
		}
		constexpr ReferenceCounter* decrement() noexcept {
			--m_counter;
			return this;
		}
		constexpr counter_type counter() const noexcept {
			return m_counter;
		}

		constexpr counter_type destroy(value_type* ptr) {
			if (m_counter == 1) {
				m_deleter->destroy(ptr);
			}

			return --m_counter;
		}

	private:
		counter_type m_counter = 1;
		deleter_type m_deleter = nullptr;
	};

public:
	using value_type = std::remove_extent_t<Ty>;
	using reference_counter_type = ReferenceCounter;
	using reference_counter = reference_counter_type*;
	using counter_type = typename reference_counter_type::counter_type;
	using pointer = Ty*;
	using wrapper = SharedPointer;

	constexpr SharedPointer() noexcept = default;
	constexpr SharedPointer(nullpointer) noexcept { }
	template <class Other> constexpr SharedPointer(Other* ptr) { 
		m_pointer = ptr;
		if (m_pointer) m_refCount = new reference_counter_type();
	}
	template <class Other, class Deleter> constexpr SharedPointer(Other* ptr, Deleter del) requires std::is_copy_constructible_v<Deleter> { 
		m_pointer = ptr;
		if (m_pointer) m_refCount = new reference_counter_type(std::forward<Deleter>(del));
	}
	constexpr SharedPointer(const wrapper& other) : m_pointer(other.m_pointer), m_refCount(other.m_refCount) { m_refCount->increment(); }
	constexpr SharedPointer(wrapper&& other) : m_pointer(std::exchange(other.m_pointer, nullptr)), m_refCount(std::exchange(other.m_refCount, nullptr)) { }
	template <class T> constexpr SharedPointer(const SharedPointer<T>& other) : m_pointer(other.m_pointer), m_refCount(dynamic_cast<reference_counter>(other.m_refCount)) { m_refCount->increment(); }
	template <class T> constexpr SharedPointer(SharedPointer<T>&& other) : m_pointer(std::exchange(other.m_pointer, nullptr)), m_refCount(dynamic_cast<reference_counter>(std::exchange(other.m_refCount, nullptr))) { }
	template <class T> constexpr SharedPointer(UniquePointer<T>&& other) {
		auto p = std::exchange(m_pointer, std::move(other.release()));
		if (m_pointer) m_refCount = new reference_counter_type();
	}
	template <class T, class D> constexpr SharedPointer(UniquePointer<T, D>&& other) {
		auto p = std::exchange(m_pointer, std::move(other.release()));
		if (m_pointer) m_refCount = new reference_counter_type(other.deleter());
	}

	constexpr ~SharedPointer() {
		if (m_refCount && m_refCount->destroy(m_pointer) == 0) delete m_refCount;
		m_pointer = nullptr;
		m_refCount = nullptr;
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
		if (m_refCount) return m_refCount->counter();
		return 0;
	}
	DEPRECATED constexpr counter_type use_count() const noexcept {
		if (m_refCount) return m_refCount->counter();
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
};

}
