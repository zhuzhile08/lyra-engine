/*************************
 * @file Vector.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Vector implementation
 * 
 * @date 2024-02-24
 * 
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <Common/Common.h>
#include <Common/Iterator.h>

#include <initializer_list>

namespace lyra {

template <class Ty> class Vector {
public:
	using value_type = Ty;
	using reference = value_type&;
	using const_reference = const value_type&;
	using array_type = Ty*;
	using const_array_type = const Ty*;

	using size_type = size_t;
	using iterator = Iterator<Ty>;
	using const_iterator = Iterator<const Ty>; 
	using wrapper = Vector;

	constexpr Vector() noexcept : m_array(new value_type[1]), m_size(0), m_capacity(1) { }
	constexpr Vector(size_type count) { basicResize(count); }
	constexpr Vector(size_type count, const value_type& value) {
		basicResize(count);
		for (size_type i = 0; i < m_size; i++) {
			m_array[i] = value;
		}
	}
	template <class It> constexpr Vector(It first, It last) {
		basicResize(last - first);
		if (m_size > 0) std::move(first, last, begin());
	}
	constexpr Vector(const wrapper& other) {
		basicResize(other.size());
		if (m_size > 0) std::move(other.begin(), other.end(), begin());
	}
	constexpr Vector(wrapper&& other) : 
		m_array(std::exchange(other.m_array, m_array)),
		m_size(std::exchange(other.m_size, 0)),
		m_capacity(std::exchange(other.m_capacity, 0)) { }
	constexpr Vector(std::initializer_list<value_type> ilist) {
		basicResize(ilist.size());
		if (m_size > 0) std::move(ilist.begin(), ilist.end(), begin());
	}

	constexpr ~Vector() {
		delete[] m_array;
	}

	constexpr wrapper& operator=(const wrapper& other) {
		delete[] basicResize(other.size());
		if (m_size > 0) std::move(other.begin(), other.end(), begin());
		return *this;
	}
	constexpr wrapper& operator=(wrapper&& other) noexcept {
		m_array = std::exchange(other.m_array, m_array);
		m_size = other.m_size;
		m_capacity = other.m_capacity;
		return *this;
	}
	constexpr wrapper& operator=(std::initializer_list<value_type> ilist) {
		delete[] basicResize(ilist.size());
		if (m_size > 0) std::move(ilist.begin(), ilist.end(), begin());
		return *this;
	}

	constexpr void assign(size_type count, const value_type& value) {
		delete[] basicResize(count);
		for (size_type i = 0; i < m_size; i++) {
			m_array[i] = value;
		}
	}
	template <class It> constexpr void assign(It first, It last) noexcept {
		delete[] basicResize(last - first);
		if (m_size > 0) std::move(first, last, begin());
	}
	constexpr void assign(std::initializer_list<value_type> ilist) {
		delete[] basicResize(ilist.size());
		if (m_size > 0) std::move(ilist.begin(), ilist.end(), begin());
	}

	constexpr void swap(wrapper& other) {
		std::swap(m_array, other.m_array);
		std::swap(m_size, other.m_size);
		std::swap(m_capacity, other.m_capacity);
	}

	NODISCARD constexpr iterator begin() noexcept {
		return m_array[0];
	}
	NODISCARD constexpr const_iterator begin() const noexcept {
		return m_array[0];
	}
	NODISCARD constexpr const_iterator cbegin() const noexcept {
		return m_array[0];
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
	NODISCARD constexpr iterator rbegin() noexcept {
		return m_array[m_size - 1];
	}
	NODISCARD constexpr const_iterator rbegin() const noexcept {
		return m_array[m_size - 1];
	}
	NODISCARD constexpr const_iterator rcbegin() const noexcept {
		return m_array[m_size - 1];
	}
	NODISCARD constexpr iterator rend() noexcept {
		return &m_array[-1];
	}
	NODISCARD constexpr const_iterator rend() const noexcept {
		return &m_array[-1];
	}
	NODISCARD constexpr const_iterator rcend() const noexcept {
		return &m_array[-1];
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
		auto s = m_size;
		auto a = basicResize(size);
		if (a) {
			std::move(&a[0], &a[s], begin());
			delete[] a;
		}
	}
	constexpr void reserve(size_type size) noexcept {
		if (size > m_capacity) {
			m_capacity = size;
			auto a = std::exchange(m_array, new value_type[m_capacity]);
			std::move(&a[0], &a[m_size], begin());
			delete[] a;
		}
	}
	constexpr void shrinkToFit() noexcept {
		if (m_capacity > m_size) {
			m_capacity = m_size;
			auto a = std::exchange(m_array, new value_type[m_capacity]);
			std::move(&a[0], &a[m_size], begin());
			delete[] a;
		}
	}
	DEPRECATED constexpr void shrink_to_fit() noexcept {
		shrinkToFit();
	}

	constexpr iterator insert(const_iterator pos, const value_type& value) {
		auto index = pos - begin();

		resizeWithGap(index, 1);

		return &(m_array[index] = value);
	}
	constexpr iterator insert(const_iterator pos, value_type&& value) {
		auto index = pos - begin();

		resizeWithGap(index, 1);

		return &(m_array[index] = std::move(value));
	}
	constexpr iterator insert(const_iterator pos, size_type count, const value_type& value) {
		auto index = pos - begin();
		
		if (count > 0) {
			resizeWithGap(index, count);

			for (size_type i = index; i < count;) {
				m_array[i++] = value;
			}
		}
		
		return &(m_array[index]);
	}
	template <class It> constexpr iterator insert(const_iterator pos, It first, It last) {
		auto index = pos - begin();
		
		if (first != last) {
			auto size = first - last;

			resizeWithGap(index, size);

			std::move(first, last, &m_array[index]);
		}
		
		return &(m_array[index]);
	}
	constexpr iterator insert(const_iterator pos, std::initializer_list<value_type> ilist) {
		auto index = pos - begin();
		
		if (ilist.size() > 0) {
			resizeWithGap(index, ilist.size());

			std::move(ilist.begin(), ilist.end(), &m_array[index]);
		}
		
		return &(m_array[index]);
	}

	template <class... Args> constexpr iterator emplace(const_iterator pos, Args&&... args) {
		auto index = pos - begin();

		resizeWithGap(index, 1);

		auto r = new (&m_array[index]) value_type(std::forward<Args>(args)...);

		return r;
	}
	template <class... Args> constexpr reference emplaceBack(Args&&... args) {
		resize(m_size + 1);

		return *(new (&m_array[m_size - 1]) value_type(std::forward<Args>(args)...));
	}
	template <class... Args> DEPRECATED constexpr reference emplace_back(Args&&... args) {
		return emplaceBack(std::forward<Args>(args)...);
	}

	constexpr void pushBack(const value_type& value) {
		resize(m_size + 1);
		m_array[m_size - 1] = value;
	}
	constexpr void pushBack(value_type&& value) {
		resize(m_size + 1);
		m_array[m_size - 1] = std::move(value);
	}
	DEPRECATED constexpr void push_back(const value_type& value) {
		pushBack(value);
	}
	DEPRECATED constexpr void push_back(value_type&& value) {
		pushBack(std::move(value));
	}

	constexpr iterator erase(const_iterator pos) {
		auto it = begin() + (pos - begin());

		std::move(it + 1, end(), it);
		popBack();
		
		return it;
	}
	constexpr iterator erase(const_iterator first, const_iterator last) {
		auto rangeBegin = begin() + (first - begin());

		if (first != last) std::move(rangeBegin + (last - first), end(), rangeBegin);
		destroyElementsAtEnd(m_size, m_size - (last - first));

		return rangeBegin;
	}

	constexpr void popBack() noexcept {
		if (m_size > 0) {
			m_array[m_size - 1] = std::move(value_type());
			--m_size;
		}
	}
	DEPRECATED constexpr void pop_back() noexcept {
		popBack();
	}

	constexpr void clear() noexcept {
		basicResize(0);
	}

	NODISCARD constexpr size_type size() const noexcept {
		return m_size;
	}
	NODISCARD constexpr size_type maxSize() const noexcept {
		return size_t(-1);
	}
	DEPRECATED NODISCARD constexpr size_type max_size() const noexcept {
		return size_t(-1);
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

	NODISCARD constexpr const_reference at(size_type index) const {
		if (index >= m_size) throw std::out_of_range("lyra::Vector::at(): Index exceded current internal array bounds!");
		return m_array[index];
	}
	NODISCARD constexpr reference at(size_type index) {
		if (index >= m_size) throw std::out_of_range("lyra::Vector::at(): Index exceded current internal array bounds!");
		return m_array[index];
	}
	NODISCARD constexpr const_reference operator[](size_type index) const noexcept {
		return m_array[index];
	}
	NODISCARD constexpr reference operator[](size_type index) noexcept {
		return m_array[index];
	}

private:
	array_type m_array;

	size_type m_size { };
	size_type m_capacity { };

	constexpr array_type basicResize(size_type size) {
		auto s = std::exchange(m_size, size);

		if (s < m_size) {
			if (size > m_capacity) {
				if ((m_capacity *= 2) < m_size) m_capacity = m_size;
				return std::exchange(m_array, new value_type[m_capacity]);
			} 
		} else if (s > m_size) destroyElementsAtEnd(s, m_size);

		return nullptr;
	}
	constexpr void resizeAndClear(size_type size) { // exclusively for hashmap utility
		m_size = size;
		m_capacity = m_size;
		delete[] m_array;
		m_array = new value_type[m_capacity];
	}
	constexpr void resizeWithGap(size_type index, size_type count) {
		auto a = basicResize(m_size + count);
		if (a) {
			if (index != 0) std::move(&a[0], &a[index - 1], begin());
			std::move(&a[index], &a[m_size - 1], &m_array[index + count]);
		} else {
			std::move(&m_array[index], &m_array[m_size - 1], &m_array[index + count]);
		}
		delete[] a;
	} 
	constexpr void destroyElementsAtEnd(size_type oldSize, size_type newSize) {
		for (; newSize < oldSize;) {
			m_array[newSize++] = std::move(value_type());
		}
	}

	template <class, class, class, class> friend class SparseMap;
};

} // namespace lyra
