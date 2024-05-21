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
#include <Common/Utility.h>
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
	using pointer_type = Ty*;
	using const_pointer_type = const_value*;

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

	constexpr Vector() noexcept { }
	constexpr explicit Vector(const_alloc_reference alloc) : m_alloc(alloc) { }
	constexpr explicit Vector(size_type count, const_alloc_reference alloc = allocator_type()) : 
		m_alloc(alloc) {
		resize(count);
	}
	constexpr Vector(size_type count, const_reference value, const_alloc_reference alloc = allocator_type()) : 
		m_alloc(alloc) {
		resize(count, value);
	}
	template <class It> constexpr Vector(It first, It last, const_alloc_reference alloc = allocator_type()) requires isIteratorValue<It> : 
		m_alloc(alloc) {
		assign(first, last);
	}
	constexpr Vector(const_wrapper_reference other) : 
		Vector(other.m_begin, other.m_end) { }
	constexpr Vector(const_wrapper_reference other, const_alloc_reference alloc) : 
		Vector(other.m_begin, other.m_end, alloc) { }
	constexpr Vector(wrapper_rvreference other) noexcept :
		m_alloc(std::exchange(other.m_alloc, m_alloc)), 
		m_begin(std::exchange(other.m_begin, pointer_type { })),
		m_end(std::exchange(other.m_end, pointer_type { })),
		m_cap(std::exchange(other.m_cap, pointer_type { })) { }
	constexpr Vector(wrapper_rvreference other, const_alloc_reference alloc) : 
		m_alloc(alloc),
		m_begin(std::exchange(other.m_begin, pointer_type { })),
		m_end(std::exchange(other.m_end, pointer_type { })),
		m_cap(std::exchange(other.m_cap, pointer_type { }))  { }
	constexpr Vector(init_list ilist, const_alloc_reference alloc = allocator_type()) :
		Vector(ilist.begin(), ilist.end(), alloc) { }

	constexpr ~Vector() {
		destructBehind(m_begin);
		allocator_traits::deallocate(m_alloc, m_begin, m_cap - m_begin);
		
		m_begin = nullptr;
		m_end = nullptr;
		m_cap = nullptr;
	}

	constexpr wrapper_reference operator=(const_wrapper_reference other) {
		m_alloc = other.m_alloc;
		assign(other.begin(), other.end());
		return *this;
	}
	constexpr wrapper_reference operator=(wrapper_rvreference other) noexcept {
		std::swap(other.m_alloc, m_alloc);
		std::swap(other.m_begin, m_begin);
		std::swap(other.m_end, m_end);
		std::swap(other.m_cap, m_cap);
		return *this;
	}
	constexpr wrapper_reference operator=(init_list ilist) {
		assign(ilist.begin(), ilist.end());
		return *this;
	}

	constexpr void assign(size_type count, const_reference value) {
		clear();
		resize(count, value);
	}
	template <class It> constexpr void assign(It first, It last) requires isIteratorValue<It> {
		clear();

		if (first != last) {
			auto count = last - first;
			smartReserve(count);
			m_end = m_begin + count;

			auto it = m_begin;
			while (first != last) 
				allocator_traits::construct(m_alloc, it++, *(first++));
		}
	}
	constexpr void assign(init_list ilist) {
		assign(ilist.begin(), ilist.end());
	}

	constexpr void swap(wrapper_reference other) {
		std::swap(m_begin, other.m_begin);
		std::swap(m_end, other.m_end);
		std::swap(m_cap, other.m_cap);
	}

	NODISCARD constexpr iterator begin() noexcept {
		return m_begin;
	}
	NODISCARD constexpr const_iterator begin() const noexcept {
		return m_begin;
	}
	NODISCARD constexpr const_iterator cbegin() const noexcept {
		return m_begin;
	}
	NODISCARD constexpr iterator end() noexcept {
		return m_end;
	}
	NODISCARD constexpr const_iterator end() const noexcept {
		return m_end;
	}
	NODISCARD constexpr const_iterator cend() const noexcept {
		return m_end;
	}
	NODISCARD constexpr reverse_iterator rbegin() noexcept {
		return m_end ? m_end - 1 : nullptr;
	}
	NODISCARD constexpr const_reverse_iterator rbegin() const noexcept {
		return m_end ? m_end - 1 : nullptr;
	}
	NODISCARD constexpr const_reverse_iterator crbegin() const noexcept {
		return m_end ? m_end - 1 : nullptr;
	}
	NODISCARD constexpr reverse_iterator rend() noexcept {
		return m_begin ? m_begin - 1 : nullptr;
	}
	NODISCARD constexpr const_reverse_iterator rend() const noexcept {
		return m_begin ? m_begin - 1 : nullptr;
	}
	NODISCARD constexpr const_reverse_iterator crend() const noexcept {
		return m_begin ? m_begin - 1 : nullptr;
	}

	NODISCARD constexpr reference front() noexcept {
		return *m_begin;
	}
	NODISCARD constexpr const_reference front() const noexcept {
		return *m_begin;
	}
	NODISCARD constexpr reference back() noexcept {
		return *(m_end - 1);
	}
	NODISCARD constexpr const_reference back() const noexcept {
		return *(m_end - 1);
	}
	
	constexpr void resize(size_type count) {
		auto s = size();
		if (count > s)
			append(count - s);
		else if (count < s) 
			destructBehind(m_begin + count);
	}
	constexpr void resize(size_type count, const_reference value) {
		auto s = size();
		if (count > s)
			append(count - s, value);
		else if (count < s) 
			destructBehind(m_begin + count);
	}
	constexpr void reserve(size_type count) {
		auto cap = capacity();

		if (count > cap) {
			auto s = this->size();
			auto oldBegin = std::exchange(m_begin, allocator_traits::allocate(m_alloc, count));
			auto oldEnd = std::exchange(m_end, m_begin + s);
			m_cap = m_begin + count;

			if (oldBegin) {
				auto beginIt = m_begin;
				auto oldBeginIt = oldBegin;

				while (oldBeginIt != oldEnd)
					allocator_traits::construct(m_alloc, beginIt++, std::move(*(oldBeginIt++)));

				allocator_traits::deallocate(m_alloc, oldBegin, cap);
			}
		}
	}
	constexpr void shrinkToFit() {
		auto s = size();
		auto cap = capacity();

		if (s < cap) {
			auto oldBegin = std::exchange(m_begin, allocator_traits::allocate(m_alloc, s));
			auto oldEnd = std::exchange(m_end, m_begin + s);

			if (oldBegin) {
				std::move(oldBegin, oldEnd, m_begin);
				allocator_traits::deallocate(m_alloc, oldBegin, cap);
			}
		}
	}
	DEPRECATED constexpr void shrink_to_fit() {
		shrinkToFit();
	}

	constexpr iterator insert(const_iterator position, const_reference value) {
		return constructBehind(position, value);
	}
	constexpr iterator insert(const_iterator position, rvreference value) {
		return constructBehind(position, std::move(value));
	}
	constexpr iterator insert(const_iterator position, size_type count, const_reference value) {
		if (count != 0) {
			// calculate general size and capacity information
			auto minReserveCount = size() + count;
			auto cap = capacity();

			// convert position iterator to index
			auto index = position - m_begin;

			if (minReserveCount > cap) {
				// reserve memory without constructing new memory, similar to smartReserve()
				auto doubleCap = cap * 2;
				auto reserveCount = (minReserveCount > doubleCap) ? minReserveCount : doubleCap;
				auto oldBegin = std::exchange(m_begin, allocator_traits::allocate(m_alloc, reserveCount));
				
				// calculate new iterators
				m_end = m_begin + minReserveCount;
				m_cap = m_begin + reserveCount;

				// prepare some iterators for the following parts
				auto pos = m_begin + index; // can't use position due to memory invalidation
				auto it = m_begin;

				// re-construct the vector in front of pos/position
				while (it < pos && oldBegin)
					allocator_traits::construct(m_alloc, it++, std::move(*(oldBegin++)));

				// construct the in-between section from begin and end
				for (; count > 0; count--)
					allocator_traits::construct(m_alloc, it++, value);

				// reconstruct the remaining parts of the vector
				while (it < m_end && oldBegin)
					allocator_traits::construct(m_alloc, it++, std::move(*(oldBegin++)));
				
				return pos;
			} else {
				auto oldEnd = std::exchange(m_end, m_end + count);
				auto endIt = m_end;
				auto pos = m_begin + index; // convert to non-const iterator

				for (auto c = count; c > 0; c--) 
					allocator_traits::construct(m_alloc, --endIt, std::move(*(--oldEnd)));

				std::move_backward(pos, oldEnd, endIt);

				std::fill_n(pos, count, value);
				
				return pos;
			}
		} else {
			return { };
		}
	}
	template <class It> constexpr iterator insert(const_iterator position, It first, It last) requires isIteratorValue<It> {
		if (first != last) {
			// calculate general size and capacity information
			auto count = last - first;
			auto minReserveCount = size() + count;
			auto cap = capacity();

			// convert position iterator to index
			auto index = position - m_begin;

			if (minReserveCount > cap) {
				// reserve memory without constructing new memory, similar to smartReserve()
				auto doubleCap = cap * 2;
				auto reserveCount = (minReserveCount > doubleCap) ? minReserveCount : doubleCap;
				auto oldBegin = std::exchange(m_begin, allocator_traits::allocate(m_alloc, reserveCount));
				
				// calculate new iterators
				m_end = m_begin + minReserveCount;
				m_cap = m_begin + reserveCount;

				// prepare some iterators for the following parts
				auto pos = m_begin + index; // can't use position due to memory invalidation
				auto it = m_begin;

				// re-construct the vector in front of pos/position
				while (it < pos && oldBegin)
					allocator_traits::construct(m_alloc, it++, std::move(*(oldBegin++)));

				// construct the in-between section from begin and end
				while (first != last)
					allocator_traits::construct(m_alloc, it++, std::move(*(first++)));

				// reconstruct the remaining parts of the vector
				while (it < m_end && oldBegin)
					allocator_traits::construct(m_alloc, it++, std::move(*(oldBegin++)));

				return pos;
			} else {
				auto oldEnd = std::exchange(m_end, m_end + count);
				auto endIt = m_end;
				auto pos = m_begin + index; // convert to non-const iterator

				for (; count > 0; count--)
					allocator_traits::construct(m_alloc, --endIt, std::move(*(--oldEnd)));

				std::move_backward(pos, oldEnd, endIt);

				std::copy(first, last, pos);

				return pos;
			}
		} else {
			return { };
		}
	}
	constexpr iterator insert(const_iterator pos, init_list ilist) {
		return insert(pos, ilist.begin(), ilist.end());
	}

	template <class... Args> constexpr iterator emplace(const_iterator pos, Args&&... args) {
		return constructBehind(pos, std::forward<Args>(args)...);
	}
	template <class... Args> constexpr reference emplaceBack(Args&&... args) {
		smartReserve(size() + 1);
		allocator_traits::construct(m_alloc, m_end, std::forward<Args>(args)...);
		return *m_end++;
	}
	template <class... Args> DEPRECATED constexpr reference emplace_back(Args&&... args) {
		return emplaceBack(std::forward<Args>(args)...);
	}

	constexpr void pushBack(const_reference value) {
		append(1, value);
	}
	constexpr void pushBack(rvreference value) {
		smartReserve(size() + 1);
		allocator_traits::construct(m_alloc, m_end++, std::move(value));
	}
	DEPRECATED constexpr void push_back(const_reference value) {
		pushBack(value);
	}
	DEPRECATED constexpr void push_back(rvreference value) {
		pushBack(std::move(value));
	}

	constexpr iterator erase(const_iterator pos) {
		ASSERT(pos < end(), "Lyra::Vector::erase: past-end iterator passed to erase!");

		auto it = m_begin + (pos - m_begin);
		allocator_traits::destroy(m_alloc, &*it);

		allocator_traits::construct(m_alloc, &*it, std::move(*(it + 1))); // weird bit here because it needs to reconstruct the previously destroyed element
		std::move(it + 2, m_end, it + 1);

		popBack();

		return it;
	}
	constexpr iterator erase(const_iterator first, const_iterator last) {
		auto firstIt = m_begin + (first - m_begin);

		if (first < last) {
			auto distance = last - first;
			auto lastIt = firstIt + distance;

			auto it = firstIt;	

			if (distance >= (m_end - last)) { // if there are more deleted elements than elements at the end, the while loop is centered around moving the end into the deleted section
				while (lastIt != m_end) {
					allocator_traits::destroy(m_alloc, it);
					allocator_traits::construct(m_alloc, it++, std::move(*(lastIt++)));
				}
			} else { // otherwise focus on getting the entire secon deleted first, then move the remaining elements at the end
				while (it != last) {
					allocator_traits::destroy(m_alloc, it);
					allocator_traits::construct(m_alloc, it++, std::move(*(lastIt++)));
				}

				std::move(lastIt, m_end, it);
			}

			destructBehind(m_end - distance);
		}

		return firstIt;
	}

	constexpr void popBack() {
		destructBehind(m_end - 1);
	}
	DEPRECATED constexpr void pop_back() {
		popBack();
	}

	constexpr void clear() {
		destructBehind(m_begin);
	}

	NODISCARD constexpr size_type size() const noexcept {
		return m_end - m_begin;
	}
	NODISCARD constexpr size_type maxSize() const noexcept {
		return std::min<size_type>(-1, allocator_traits::maxSize(m_alloc));
	}
	DEPRECATED NODISCARD constexpr size_type max_size() const noexcept {
		return maxSize();
	}
	NODISCARD constexpr size_type capacity() const noexcept {
		return m_cap - m_begin;
	}
	NODISCARD constexpr bool empty() const noexcept {
		return m_begin == m_end;
	}

	NODISCARD constexpr const_pointer_type data() const noexcept {
		return m_begin;
	}
	NODISCARD constexpr pointer_type data() noexcept {
		return m_begin;
	}

	NODISCARD constexpr allocator_type allocator() const noexcept {
		return m_alloc;
	}
	DEPRECATED NODISCARD constexpr allocator_type get_allocator() const noexcept {
		return allocator();
	}

	NODISCARD constexpr const_reference at(size_type index) const {
		auto ptr = m_begin + index;
		if (ptr >= m_end) throw std::out_of_range("lyra::Vector::at(): Index exceded current internal array bounds!");
		return *ptr;
	}
	NODISCARD constexpr reference at(size_type index) {
		auto ptr = m_begin + index;
		if (ptr >= m_end) throw std::out_of_range("lyra::Vector::at(): Index exceded current internal array bounds!");
		return *ptr;
	}
	NODISCARD constexpr const_reference operator[](size_type index) const {
		auto ptr = m_begin + index;
		ASSERT(ptr < m_end, "lyra::Vector::operator[]: Index of: {} exceded array size of: {}!", index, size());
		return *ptr;
	}
	NODISCARD constexpr reference operator[](size_type index) {
		auto ptr = m_begin + index;
		ASSERT(ptr < m_end, "lyra::Vector::operator[]: Index of: {} exceded array size of: {}!", index, size());
		return *ptr;
	}

private:
	NO_UNIQUE_ADDRESS allocator_type m_alloc { };

	pointer_type m_begin { };
	pointer_type m_end { };
	pointer_type m_cap { };

	constexpr void smartReserve(size_type size) noexcept {
		auto cap = capacity();

		if (size > cap) {
			auto newCap = cap * 2;
			reserve((newCap < size) ? size : newCap);
		}
	}
	constexpr void resizeAndClear(size_type size) noexcept { // exclusively for hashmap utility
		clear();
		resize(size);
	}
	
	constexpr void append(size_type count) noexcept {
		smartReserve(size() + count);

		for (; count > 0; count--) allocator_traits::construct(m_alloc, m_end++);
	}
	constexpr void append(size_type count, const_reference value) noexcept {
		smartReserve(size() + count);
		
		for (; count > 0; count--) allocator_traits::construct(m_alloc, m_end++, value);
	}

	template <class... Args, class DstIt> constexpr iterator constructBehind(DstIt position, Args&&... args) noexcept requires isIteratorValue<DstIt> {
		// calculate general size and capacity information
		auto minReserveCount = size() + 1;
		auto cap = capacity();

		// convert position iterator to index
		auto index = position - m_begin;

		if (minReserveCount > cap) {
			// reserve memory without constructing new memory, similar to smartReserve()
			auto reserveCount = std::max(cap * 2, size_type(1));
			auto oldBegin = std::exchange(m_begin, allocator_traits::allocate(m_alloc, reserveCount));
			
			// calculate new iterators
			m_end = m_begin + minReserveCount;
			m_cap = m_begin + reserveCount;

			// prepare some iterators for the following parts
			auto pos = m_begin + index; // can't use position due to memory invalidation
			auto it = m_begin;

			// re-construct the vector in front of pos/position
			while (it < pos && oldBegin)
				allocator_traits::construct(m_alloc, it++, std::move(*(oldBegin++)));

			// construct the in-between element from begin and end
			allocator_traits::construct(m_alloc, it++, std::forward<Args>(args)...);

			// reconstruct the remaining parts of the vector
			while (it < m_end && oldBegin)
				allocator_traits::construct(m_alloc, it++, std::move(*(oldBegin++)));
			
			return pos;
		} else {
			auto oldEnd = m_end++;
			auto pos = m_begin + index; // convert to non-const iterator

			if (pos < oldEnd || m_end > m_begin) { // edge case, there is probably a better way to do this
				allocator_traits::construct(m_alloc, oldEnd--, std::move(*oldEnd));
				std::move_backward(pos, oldEnd, oldEnd + 1);
			}

			allocator_traits::construct(m_alloc, pos, std::forward<Args>(args)...);

			return pos;
		}
	}

	template <class It> constexpr void destructBehind(It position) requires isIteratorValue<It> {
		while (m_end != position) allocator_traits::destroy(m_alloc, --m_end);
	}

	template <class, class, class, class, class> friend class UnorderedSparseMap;
	template <class, class, class, class> friend class UnorderedSparseSet;
};

} // namespace lyra
