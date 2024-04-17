/*************************
 * @file Vector.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Vector implementation
 * 
 * @date 2024-02-24
 * 
 * @copyright Copyright (c) 2024
 *************************/

#pragma once

#include <Common/Common.h>
#include <Common/Allocator.h>
#include <Common/Iterators.h>

#include <cstdlib>
#include <new>
#include <utility>
#include <initializer_list>

namespace lyra {

template <class Ty, class Alloc = std::allocator<Ty>> class Vector { // @todo custom compile time allocator implementation
public:
	using allocator_type = Alloc;
	using const_alloc_reference = const allocator_type&;
	using allocator_traits = AllocatorTraits<allocator_type>;

	using value_type = Ty;
	using const_value = const value_type;
	using reference = value_type&;
	using const_reference = const_value&;
	using rvreference = value_type&&;
	using array_type = Ty*;
	using const_array_type = const_value*;

	using size_type = size_type;
	using iterator = Iterator<value_type>;
	using const_iterator = Iterator<const_value>; 
	using reverse_iterator = ReverseIterator<value_type>;
	using const_reverse_iterator = ReverseIterator<const_value>; 

	using wrapper = Vector;
	using wrapper_reference = wrapper&;
	using const_wrapper_reference = const wrapper&;
	using wrapper_rvreference = wrapper&&;
	using init_list = std::initializer_list<value_type>;

	constexpr Vector() noexcept : m_array(nullptr), m_size(0), m_capacity(0) { }
	constexpr explicit Vector(const_alloc_reference alloc) :
		m_alloc(alloc),
		m_array(nullptr), 
		m_size(0), 
		m_capacity(0) { }
	constexpr explicit Vector(size_type count, const_alloc_reference alloc = allocator_type()) : 
		m_alloc(alloc),
		m_array(allocator_traits::allocate(m_alloc, count)),
		m_size(count), 
		m_capacity(count) {
		if (count > 0) assignImpl(count);
	}
	constexpr Vector(size_type count, const_reference value, const_alloc_reference alloc = allocator_type()) : 
		m_alloc(alloc), 
		m_array(allocator_traits::allocate(m_alloc, count)), 
		m_size(count), 
		m_capacity(count) {
		if (count > 0) assignImpl(count, value);
	}
	template <class It> constexpr Vector(It first, It last, const_alloc_reference alloc = allocator_type()) requires isIteratorValue<It> : 
		m_alloc(alloc),
		m_array(allocator_traits::allocate(m_alloc, last - first)), 
		m_size(last - first), 
		m_capacity(last - first) {
		if (m_size > 0) assignImpl(first, last);
	}
	constexpr Vector(const_wrapper_reference other) : 
		m_alloc(other.m_alloc), 
		m_array(allocator_traits::allocate(m_alloc, other.size())), 
		m_size(other.size()), 
		m_capacity(other.size()) {
		if (m_size > 0) assignImpl(other.begin(), other.end());
	}
	constexpr Vector(const_wrapper_reference other, const_alloc_reference alloc) : 
		m_alloc(alloc), 
		m_array(allocator_traits::allocate(m_alloc, other.size())), 
		m_size(other.size()), 
		m_capacity(other.size()) {
		if (m_size > 0) assignImpl(other.begin(), other.end());
	}
	constexpr Vector(wrapper_rvreference other) noexcept :
		m_alloc(std::exchange(other.m_alloc, m_alloc)), 
		m_array(std::exchange(other.m_array, nullptr)),
		m_size(std::exchange(other.m_size, 0)),
		m_capacity(std::exchange(other.m_capacity, 0)) { }
	constexpr Vector(wrapper_rvreference other, const_alloc_reference alloc) : 
		m_alloc(alloc),
		m_array(std::exchange(other.m_array, nullptr)),
		m_size(std::exchange(other.m_size, 0)),
		m_capacity(std::exchange(other.m_capacity, 0)) { }
	constexpr Vector(init_list ilist, const_alloc_reference alloc = allocator_type()) : 
		m_alloc(alloc),
		m_array(allocator_traits::allocate(m_alloc, ilist.size())), 
		m_size(ilist.size()), 
		m_capacity(ilist.size()) {
		assignImpl(ilist.begin(), ilist.end());
	}

	constexpr ~Vector() {
		if (m_size != 0) {
			destroyElementsAtEnd(&m_array[0]);
		}
		allocator_traits::deallocate(m_alloc, m_array, m_capacity);
		m_array = nullptr;
		m_capacity = 0;
		m_size = 0;
	}

	constexpr wrapper& operator=(const_wrapper_reference other) {
		m_alloc = other.m_alloc;
		assign(other.begin(), other.end());
		return *this;
	}
	constexpr wrapper& operator=(wrapper_rvreference other) noexcept {
		std::swap(other.m_alloc, m_alloc);
		std::swap(other.m_array, m_array);
		std::swap(other.m_size, m_size);
		std::swap(other.m_capacity, m_capacity);
		return *this;
	}
	constexpr wrapper& operator=(init_list ilist) {
		assign(ilist.begin(), ilist.end());
		return *this;
	}

	constexpr void assign(size_type count, const_reference value) {
		clear();
		resize(count, value);
	}
	template <class It> constexpr void assign(It first, It last) noexcept requires isIteratorValue<It> {
		clear();
		auto newSize = last - first;
		smartReserve(newSize);
		m_size = newSize;
		assignImpl(first, last);
	}
	constexpr void assign(init_list ilist) {
		assign(ilist.begin(), ilist.end());
	}

	constexpr void swap(wrapper& other) {
		std::swap(m_array, other.m_array);
		std::swap(m_size, other.m_size);
		std::swap(m_capacity, other.m_capacity);
	}

	NODISCARD constexpr iterator begin() noexcept {
		return &m_array[0];
	}
	NODISCARD constexpr const_iterator begin() const noexcept {
		return &m_array[0];
	}
	NODISCARD constexpr const_iterator cbegin() const noexcept {
		return &m_array[0];
	}
	NODISCARD constexpr iterator end() noexcept {
		return &m_array[m_size];
	}
	NODISCARD constexpr const_iterator end() const noexcept {
		return &m_array[m_size];
	}
	NODISCARD constexpr const_iterator cend() const noexcept {
		return &m_array[m_size];
	}
	NODISCARD constexpr reverse_iterator rbegin() noexcept {
		return (&m_array[(m_size == 0) ? 0 : (m_size - 1)]);
	}
	NODISCARD constexpr const_reverse_iterator rbegin() const noexcept {
		return (&m_array[(m_size == 0) ? 0 : (m_size - 1)]);
	}
	NODISCARD constexpr const_reverse_iterator crbegin() const noexcept {
		return (&m_array[(m_size == 0) ? 0 : (m_size - 1)]);
	}
	NODISCARD constexpr reverse_iterator rend() noexcept {
		return (&m_array[0]) - 1;
	}
	NODISCARD constexpr const_reverse_iterator rend() const noexcept {
		return (&m_array[0]) - 1;
	}
	NODISCARD constexpr const_reverse_iterator crend() const noexcept {
		return (&m_array[0]) - 1;
	}

	NODISCARD constexpr reference front() noexcept {
		return m_array[0];
	}
	NODISCARD constexpr const_reference front() const noexcept {
		return m_array[0];
	}
	NODISCARD constexpr reference back() noexcept {
		return m_array[m_size - 1];
	}
	NODISCARD constexpr const_reference back() const noexcept {
		return m_array[m_size - 1];
	}
	
	constexpr void resize(size_type size) noexcept {
		if (size > m_size) {
			smartReserve(size);
			for (auto it = &m_array[m_size]; it != &m_array[size]; it++) allocator_traits::construct(m_alloc, it);
		} else destroyElementsAtEnd(&m_array[size]);
		m_size = size;
	}
	constexpr void resize(size_type size, const_reference value) noexcept {
		if (size > m_size) {
			smartReserve(size);
			for (auto it = &m_array[m_size]; it != &m_array[size]; it++) allocator_traits::construct(m_alloc, it, value);
		} else destroyElementsAtEnd(&m_array[size]);
		m_size = size;
	}
	constexpr void reserve(size_type size) noexcept {
		if (size > m_capacity) {
			array_type a = m_array;
			m_array = allocator_traits::allocate(m_alloc, size);
			if (a) {
				assignImpl(&a[0], &a[m_size]);
				allocator_traits::deallocate(m_alloc, a, m_capacity);
			}
			m_capacity = size;
		}
	}
	constexpr void shrinkToFit() noexcept {
		if (m_size < m_capacity) {
			array_type a = m_array;
			m_array = allocator_traits::allocate(m_alloc, m_size);
			if (a) {
				assignImpl(&a[0], &a[m_size]);
				allocator_traits::deallocate(m_alloc, a, m_capacity);
			}
			m_capacity = m_size;
		}
	}
	DEPRECATED constexpr void shrink_to_fit() noexcept {
		shrinkToFit();
	}

	constexpr iterator insert(const_iterator pos, const_reference value) {
		auto it = resizeWithGap(pos - begin(), 1);
		allocator_traits::construct(m_alloc, &*it, value);

		return it;
	}
	constexpr iterator insert(const_iterator pos, rvreference value) {
		auto it = resizeWithGap(pos - begin(), 1);
		allocator_traits::construct(m_alloc, &*it, std::move(value));

		return it;
	}
	constexpr iterator insert(const_iterator pos, size_type count, const_reference value) {
		auto index = pos - &m_array[0];
		
		if (count > 0) {
			for (auto it = resizeWithGap(index, count); it != &m_array[index + count]; it++)
				allocator_traits::construct(m_alloc, &*it, value);
		}
		
		return &m_array[index];
	}
	template <class It> constexpr iterator insert(const_iterator pos, It first, It last) requires isIteratorValue<It> {
		auto index = pos - begin();

		if (first != last) {
			auto size = last - first;

			for (auto it = resizeWithGap(index, size); it != &m_array[index + size]; it++, first++)
				allocator_traits::construct(m_alloc, &*it, *first);
		}
		
		return &m_array[index];
	}
	constexpr iterator insert(const_iterator pos, init_list ilist) {
		return insert(pos, ilist.begin(), ilist.end());
	}

	template <class... Args> constexpr iterator emplace(const_iterator pos, Args&&... args) {
		auto it = resizeWithGap(pos - begin(), 1);
		allocator_traits::construct(m_alloc, &*it, std::forward<Args>(args)...);

		return it;
	}
	template <class... Args> constexpr reference emplaceBack(Args&&... args) {
		smartReserve(m_size + 1);
		allocator_traits::construct(m_alloc, &m_array[m_size++], std::forward<Args>(args)...);
		return back();
	}
	template <class... Args> DEPRECATED constexpr reference emplace_back(Args&&... args) {
		return emplaceBack(std::forward<Args>(args)...);
	}

	constexpr void pushBack(const_reference value) {
		smartReserve(m_size + 1);
		allocator_traits::construct(m_alloc, &m_array[m_size++], value);
	}
	constexpr void pushBack(rvreference value) {
		smartReserve(m_size + 1);
		allocator_traits::construct(m_alloc, &m_array[m_size++], std::move(value));
	}
	DEPRECATED constexpr void push_back(const_reference value) {
		pushBack(value);
	}
	DEPRECATED constexpr void push_back(rvreference value) {
		pushBack(std::move(value));
	}

	constexpr iterator erase(const_iterator pos) {
		ASSERT(pos != end(), "Lyra::Vector::erase: past-end iterator passed to erase!");
		auto it = begin() + (pos - begin());
		allocator_traits::destroy(m_alloc, &*it);

		std::move(it + 1, end(), it);
		popBack();

		return it;
	}
	constexpr iterator erase(const_iterator first, const_iterator last) {
		auto rangeBegin = &m_array[0] + (first - begin());

		if (first < last) {
			auto distance = last - first;
			auto rangeEnd = rangeBegin + distance;

			for (auto rangeIt = rangeBegin, it = rangeEnd; it != &m_array[m_size]; rangeIt++, it++)
				*rangeIt = std::move(*it);

			destroyElementsAtEnd(&m_array[m_size -= distance]);
		}

		return rangeBegin;
	}

	constexpr void popBack() noexcept {
		if (m_size > 0) {
			--m_size;
			allocator_traits::destroy(m_alloc, &m_array[m_size]);
		}
	}
	DEPRECATED constexpr void pop_back() noexcept {
		popBack();
	}

	constexpr void clear() noexcept {
		destroyElementsAtEnd(&m_array[0]);
		m_size = 0;
	}

	NODISCARD constexpr size_type size() const noexcept {
		return m_size;
	}
	NODISCARD constexpr size_type maxSize() const noexcept {
		return std::min<size_type>(-1, allocator_traits::maxSize(m_alloc));
	}
	DEPRECATED NODISCARD constexpr size_type max_size() const noexcept {
		return maxSize();
	}
	NODISCARD constexpr size_type capacity() const noexcept {
		return m_capacity;
	}
	NODISCARD constexpr bool empty() const noexcept {
		return m_size == 0;
	}

	NODISCARD constexpr const_array_type data() const noexcept {
		return m_array;
	}
	NODISCARD constexpr array_type data() noexcept {
		return m_array;
	}

	NODISCARD constexpr allocator_type allocator() const noexcept {
		return m_alloc;
	}
	DEPRECATED NODISCARD constexpr allocator_type get_allocator() const noexcept {
		return allocator();
	}

	NODISCARD constexpr const_reference at(size_type index) const {
		if (index >= m_size) throw std::out_of_range("lyra::Vector::at(): Index exceded current internal array bounds!");
		return m_array[index];
	}
	NODISCARD constexpr reference at(size_type index) {
		if (index >= m_size) throw std::out_of_range("lyra::Vector::at(): Index exceded current internal array bounds!");
		return m_array[index];
	}
	NODISCARD constexpr const_reference operator[](size_type index) const noexcept {
		ASSERT(index < m_size, "lyra::Vector::operator[]: Index of: {} exceded array size of: {}!", index, m_size);
		return m_array[index];
	}
	NODISCARD constexpr reference operator[](size_type index) noexcept {
		ASSERT(index < m_size, "lyra::Vector::operator[]: Index of: {} exceded array size of: {}!", index, m_size);
		return m_array[index];
	}

private:
	NO_UNIQUE_ADDRESS allocator_type m_alloc { };

	array_type m_array { };

	size_type m_size { };
	size_type m_capacity { };

	template <class It> constexpr void assignImpl(It first, It last) noexcept requires isIteratorValue<It> {
		if constexpr (std::is_copy_constructible_v<value_type>) for (auto it = &m_array[0]; first != last; first++, it++) allocator_traits::construct(m_alloc, it, *first);
		else for (auto it = &m_array[0]; first != last; first++, it++) allocator_traits::construct(m_alloc, it, std::move(*first));
	}
	constexpr void assignImpl(size_type count, const_reference value) noexcept {
		for (auto it = &m_array[0]; it < &m_array[count]; it++) allocator_traits::construct(m_alloc, it, value);
	}
	constexpr void assignImpl(size_type count) noexcept {
		for (auto it = &m_array[0]; it < &m_array[count]; it++) allocator_traits::construct(m_alloc, it);
	}

	constexpr void smartReserve(size_type size) noexcept {
		if (size > m_capacity) {
			auto c = m_capacity * 2;
			reserve((c < size) ? size : c);
		}
	}
	constexpr void resizeAndClear(size_type size) noexcept { // exclusively for hashmap utility
		clear();
		resize(size);
	}
	constexpr iterator resizeWithGap(size_type index, size_type count) noexcept {
		if (m_size != 0) {
			auto newSize = m_size + count;
			smartReserve(newSize);

			auto rfirst = &m_array[m_size] - 1;
			auto rlast = &m_array[index] - 1;
			auto newBack = rfirst + count;

			for (; rfirst != rlast; rfirst--, newBack--) {
				allocator_traits::construct(m_alloc, newBack, std::move(*rfirst));
				allocator_traits::destroy(m_alloc, rfirst);
			}

			m_size = newSize;
			return ++rlast;
		} else {
			resize(count);
			return &m_array[0];
		}
		return m_array;
	}

	template <class It> constexpr void destroyElementsAtEnd(It position) noexcept requires isIteratorValue<It> {
		for (; position != &m_array[m_size]; position++) allocator_traits::destroy(m_alloc, position);
	}

	template <class, class, class, class, class> friend class SparseMap;
	template <class, class, class, class, class> friend class UnorderedSparseMap;
	template <class, class, class, class> friend class SparseSet;
	template <class, class, class, class> friend class UnorderedSparseSet;
};

} // namespace lyra
