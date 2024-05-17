/*************************
 * @file UnorderedSparseSet.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Unordered Sparse Set implementation
 * 
 * @date 2024-02-24
 * 
 * @copyright Copyright (c) 2024
 *************************/

#pragma once

#include <Common/Common.h>
#include <Common/Utility.h>
#include <Common/Iterators.h>
#include <Common/Vector.h>
#include <Common/Hash.h>
#include <Common/Allocator.h>
#include <Common/ForwardList.h>

#include <initializer_list>
#include <functional>
#include <utility>

namespace lyra {

template <
	class Key,
	class Hash = Hash<Key>,
	class Equal = std::equal_to<Key>,
	class Alloc = std::allocator<Key>
> class UnorderedSparseSet {
public:
	static constexpr float maxLoadFactor = 2;

	using key_type = Key;
	using allocator_type = Alloc;
	template <class F, class S> using pair_type = std::pair<F, S>;

	using value_type = key_type;
	using reference = value_type&;
	using const_reference = const value_type&;
	using rvreference = value_type&&;
	using pointer = value_type*;
	using const_pointer = const pointer;
	using array = Vector<value_type>;

	using bucket_type = size_type;
	using bucket_pointer = bucket_type*;
	using bucket_list = ForwardList<bucket_type>;
	using buckets = Vector<bucket_list>;

	using iterator = typename array::iterator;
	using const_iterator = typename array::const_iterator;
	using bucket_iterator = typename buckets::iterator;
	using const_bucket_iterator = typename buckets::const_iterator;

	using hasher = Hash;
	using key_equal = Equal;

	using wrapper = UnorderedSparseSet;
	using const_wrapper_reference = const wrapper&;
	using wrapper_rvreference = wrapper&&;

	constexpr UnorderedSparseSet() noexcept : m_buckets(2) { } 
	explicit constexpr UnorderedSparseSet(
		size_type bucketCount, 
		const hasher& hash = hasher(), 
		const key_equal& keyEqual = key_equal(), 
		const allocator_type& alloc = allocator_type()) noexcept : 
		m_array(alloc),
		m_buckets(hashmapBucketSizeCheck(bucketCount, 2)), 
		m_hasher(hash), 
		m_equal(keyEqual) { } 
	constexpr UnorderedSparseSet(size_type bucketCount, const allocator_type& alloc) noexcept : 
		m_array(alloc), m_buckets(hashmapBucketSizeCheck(bucketCount, 2)) { } 
	constexpr UnorderedSparseSet(size_type bucketCount, const hasher& hasher, const allocator_type& alloc) noexcept : 
		m_array(alloc), m_buckets(hashmapBucketSizeCheck(bucketCount, 2)), m_hasher(hasher) { } 
	explicit constexpr UnorderedSparseSet(const allocator_type& alloc) noexcept : 
		m_array(alloc), m_buckets(2) { } 
	template <class It> constexpr UnorderedSparseSet(
		It first, It last, 
		size_type bucketCount = 0, // set to 0 for default evaluation
		const hasher& hash = hasher(), 
		const key_equal& keyEqual = key_equal(), 
		const allocator_type& alloc = allocator_type()) noexcept requires isIteratorValue<It> : 
		m_array(alloc),
		m_buckets(hashmapBucketSizeCheck(bucketCount, last - first)), 
		m_hasher(hash), 
		m_equal(keyEqual) {
		insert(first, last);
	}
	template <class It> constexpr UnorderedSparseSet(
		It first, It last, size_type bucketCount, const allocator_type& alloc) noexcept 
		requires isIteratorValue<It> : m_array(alloc), m_buckets(hashmapBucketSizeCheck(bucketCount, last - first)) {
		insert(first, last);
	}
	template <class It> constexpr UnorderedSparseSet(
		It first, It last, 
		size_type bucketCount,
		const hasher& hasher,
		const allocator_type& alloc) noexcept requires isIteratorValue<It> : 
		m_array(alloc),
		m_buckets(hashmapBucketSizeCheck(bucketCount, last - first)), 
		m_hasher(hasher) {
		insert(first, last);
	}
	constexpr UnorderedSparseSet(const_wrapper_reference other) : 
		m_array(other.m_array), m_buckets(other.m_buckets) { }
	constexpr UnorderedSparseSet(const_wrapper_reference other, const allocator_type& alloc) :
		m_array(other.m_array, alloc), m_buckets(other.m_buckets) { }
	constexpr UnorderedSparseSet(wrapper_rvreference other) noexcept :
		m_array(std::move(other.m_array)), m_buckets(std::move(other.m_buckets)) { }
	constexpr UnorderedSparseSet(wrapper_rvreference other, const allocator_type& alloc) : 
		m_array(std::move(other.m_array), alloc), m_buckets(std::move(other.m_buckets)) { }
	constexpr UnorderedSparseSet(
		std::initializer_list<value_type> ilist, 
		size_type bucketCount = 0, // set to 0 for default evaluation
		const hasher& hash = hasher(), 
		const key_equal& keyEqual = key_equal(), 
		const allocator_type& alloc = allocator_type()) noexcept : 
		m_array(alloc),
		m_buckets(hashmapBucketSizeCheck(bucketCount, ilist.size())), 
		m_hasher(hash), 
		m_equal(keyEqual) {
		insert(ilist.begin(), ilist.end());
	} 
	constexpr UnorderedSparseSet(std::initializer_list<value_type> ilist, size_type bucketCount, const allocator_type& alloc) noexcept : 
		m_array(alloc), m_buckets(hashmapBucketSizeCheck(bucketCount, ilist.size())) {
		insert(ilist.begin(), ilist.end());
	} 
	constexpr UnorderedSparseSet(
		std::initializer_list<value_type> ilist, size_type bucketCount, const hasher& hasher, const allocator_type& alloc) noexcept : 
		m_array(alloc), m_buckets(hashmapBucketSizeCheck(bucketCount, ilist.size())), m_hasher(hasher) {
		insert(ilist.begin(), ilist.end());
	}
	constexpr ~UnorderedSparseSet() = default;

	constexpr UnorderedSparseSet& operator=(const_wrapper_reference other) noexcept {
		std::swap(other.m_array, this->m_array);
		m_buckets = std::move(bucketsDeepCopy(other));

		return *this;
	}
	constexpr UnorderedSparseSet& operator=(wrapper_rvreference other) noexcept = default;

	constexpr void swap(wrapper& other) {
		m_array.swap(other.m_array);
		m_buckets.swap(other.m_buckets);
	}

	constexpr iterator begin() noexcept {
		return m_array.begin();
	}
	constexpr const_iterator begin() const noexcept {
		return m_array.begin();
	}
	constexpr const_iterator cbegin() const noexcept {
		return m_array.cbegin();
	}
	constexpr iterator end() noexcept {
		return m_array.end();
	}
	constexpr const_iterator end() const noexcept {
		return m_array.end();
	}
	constexpr const_iterator cend() const noexcept {
		return m_array.cend();
	}
	constexpr iterator rbegin() noexcept {
		return m_array.rbegin();
	}
	constexpr const_iterator rbegin() const noexcept {
		return m_array.rbegin();
	}
	constexpr const_iterator crbegin() const noexcept {
		return m_array.crbegin();
	}
	constexpr iterator rend() noexcept {
		return m_array.rend();
	}
	constexpr const_iterator rend() const noexcept {
		return m_array.rend();
	}
	constexpr const_iterator crend() const noexcept {
		return m_array.crend();
	}

	constexpr bucket_iterator begin(size_type index) noexcept {
		return m_buckets[index].begin();
	}
	constexpr const_bucket_iterator begin(size_type index) const noexcept {
		return m_buckets[index].begin();
	}
	constexpr const_bucket_iterator cbegin(size_type index) const noexcept {
		return m_buckets[index].cbegin();
	}
	constexpr bucket_iterator end(size_type index) noexcept {
		return m_buckets[index].end();
	}
	constexpr const_bucket_iterator end(size_type index) const noexcept {
		return m_buckets[index].end();
	}
	constexpr const_bucket_iterator cend(size_type index) const noexcept {
		return m_buckets[index].cend();
	}
	constexpr bucket_iterator rbegin(size_type index) noexcept {
		return m_buckets[index].rbegin();
	}
	constexpr const_bucket_iterator rbegin(size_type index) const noexcept {
		return m_buckets[index].rbegin();
	}
	constexpr const_bucket_iterator crbegin(size_type index) const noexcept {
		return m_buckets[index].crbegin();
	}
	constexpr bucket_iterator rend(size_type index) noexcept {
		return m_buckets[index].rend();
	}
	constexpr const_bucket_iterator rend(size_type index) const noexcept {
		return m_buckets[index].rend();
	}
	constexpr const_bucket_iterator crend(size_type index) const noexcept {
		return m_buckets[index].crend();
	}

	NODISCARD constexpr reference front() noexcept {
		return m_array.front();
	}
	NODISCARD constexpr const_reference front() const noexcept {
		return m_array.front();
	}
	NODISCARD constexpr reference back() noexcept {
		return m_array.back();
	}
	NODISCARD constexpr const_reference back() const noexcept {
		return m_array.back();
	}

	void rehash(size_type count) noexcept {
		m_buckets.resizeAndClear(count);

		size_type i = 0;
		for (auto it = m_array.begin(); it != m_array.end(); it++, i++)
			m_buckets[keyToBucket(*it)].emplaceFront(i);
	}

	constexpr pair_type<iterator, bool> insert(const_reference value) noexcept {
		auto it = find(value);

		if (it != m_array.end())
			return { it, false };
		else
			return { basicInsert(value), true };
	}
	constexpr pair_type<iterator, bool> insert(rvreference value) noexcept {
		auto it = find(value);

		if (it != m_array.end())
			return { it, false };
		else
			return { basicInsert(std::move(value)), true };
	}
	template <class K> constexpr pair_type<iterator, bool> insert(K&& obj) noexcept requires std::is_constructible_v<value_type, K&&> {
		auto it = find(obj);

		if (it != m_array.end())
			return { it, false };
		else
			return { basicInsert(std::forward<K>(obj)), true };
	}
	DEPRECATED constexpr iterator insert(const_iterator, const_reference value) noexcept {
		return insert(value).first;
	}
	DEPRECATED constexpr iterator insert(const_iterator, rvreference value) noexcept {
		return insert(std::move(value)).first;
	}
	template <class K> DEPRECATED constexpr iterator insert(const_iterator, K&& obj) noexcept requires std::is_constructible_v<value_type, K&&> {
		return insert(std::forward<K>(obj)).first;
	}
	template <class It> constexpr void insert(It first, It last) noexcept requires isIteratorValue<It> {
		for (; first < last; first++) {
			insert(*first);
		}
	}
	constexpr void insert(std::initializer_list<value_type> ilist) noexcept {
		insert(ilist.begin(), ilist.end());
	}

	template <class... Args> constexpr pair_type<iterator, bool> emplace(Args&&... args) noexcept {
		value_type v(std::forward<Args>(args)...);
		auto it = find(v);

		if (it != m_array.end()) {
			return { it, false };
		} else {
			return { basicInsert(std::move(v)), true };
		}
	}
	template <class... Args> DEPRECATED constexpr iterator emplaceHint(const_iterator, Args&&... args) noexcept {
		return emplace(std::forward<Args>(args)...);
	}
	template <class... Args> DEPRECATED constexpr iterator emplace_hint(const_iterator, Args&&... args) noexcept {
		return emplace(std::forward<Args>(args)...);
	}

	constexpr iterator erase(const_iterator pos) noexcept {
		ASSERT(pos != m_array.end(), "lyra::UnorderedSparseSet::erase(): The end iterator was passed to the function!");

		auto index = pos - m_array.begin();
		auto it = &m_array[index];

		{ // Since I don't want to find a good name for the other bucketList down below
			auto& bucketList = m_buckets[keyToBucket(*it)];

			for (auto bucketIt = bucketList.begin(), prev = bucketList.beforeBegin(); bucketIt != bucketList.end(); bucketIt++, prev++) {
				if (*bucketIt == index) {
					bucketList.eraseAfter(prev);
					break;
				}
			}
		}

		{
			auto& bucketList = m_buckets[keyToBucket(m_array.back())];

			for (auto bucketIt = bucketList.begin(); bucketIt != bucketList.end(); bucketIt++) {
				if (*bucketIt == (sizeToIndex(m_array.size()))) {
					*bucketIt = index;
					break;
				}
			}
		}

		*it = std::move(m_array.back());
		m_array.popBack();
		return it;
	}
	constexpr iterator erase(iterator pos) noexcept {
		return erase(const_iterator(&*pos));
	}
	constexpr iterator erase(const_iterator first, const_iterator last) noexcept {
		for (auto it = first; it != last; it++) erase(it);
		return &*first;
	}
	constexpr size_type erase(const key_type& key) noexcept {
		auto it = find(key);

		if (it != m_array.end()) {
			erase(it);
			return 1;
		}

		return 0;
	}
	template <class K> constexpr size_type erase(K&& key) noexcept 
		requires(!std::is_convertible_v<K, iterator> && !std::is_convertible_v<K, const_iterator>) {
		auto it = find(std::forward<K>(key));

		if (it != m_array.end()) {
			erase(it);
			return 1;
		}

		return 0;
	}

	constexpr value_type extract(const_iterator pos) noexcept {
		ASSERT(pos < m_array.end(), "lyra::UnorderedSparseMap::extract(): The end iterator was passed to the function!");

		value_type v(std::move(*const_cast<value_type*>(pos.get())));
		erase(pos);
		return v;
	}
	constexpr value_type extract(const key_type& key) noexcept {
		auto it = find(key);
		if (it < m_array.end())
			return extract(it);
		else return value_type();
	} 
	template <class K> constexpr value_type extract(K&& key) noexcept 
		requires(!std::is_convertible_v<K, iterator> && !std::is_convertible_v<K, const_iterator>) {
		auto it = find(std::forward<K>(key));
		if (it < m_array.end())
			return extract(it);
		else return value_type();
	}

	template <class OHash, class OEqual> constexpr void merge(UnorderedSparseSet<key_type, OHash, OEqual, allocator_type>& source) noexcept {
		insert(source.begin(), source.end());
	}
	template <class OHash, class OEqual> constexpr void merge(UnorderedSparseSet<key_type, OHash, OEqual, allocator_type>&& source) noexcept {
		insert(source.begin(), source.end());
	}

	constexpr void clear() noexcept {
		m_array.clear();
		m_buckets.clear();
	}

	NODISCARD constexpr size_type size() const noexcept {
		return m_array.size();
	}
	NODISCARD constexpr size_type maxSize() const noexcept {
		return m_array.maxSize();
	}
	DEPRECATED NODISCARD constexpr size_type max_size() const noexcept {
		return maxSize();
	}
	NODISCARD bool empty() const noexcept {
		return m_array.empty();
	}
	NODISCARD constexpr allocator_type allocator() const noexcept {
		return m_array.allocator();
	}
	DEPRECATED NODISCARD constexpr allocator_type get_allocator() const noexcept {
		return m_array.allocator();
	}

	NODISCARD constexpr size_type bucketCount() const noexcept {
		return m_buckets.size();
	}
	DEPRECATED NODISCARD constexpr size_type bucket_count() const noexcept {
		return m_buckets.size();
	}
	NODISCARD constexpr size_type maxBucketSize() const noexcept {
		return m_buckets.maxSize();
	}
	DEPRECATED NODISCARD constexpr size_type max_bucket_size() const noexcept {
		return m_buckets.maxSize();
	}
	NODISCARD constexpr size_type bucketSize(size_type index) const noexcept {
		return m_buckets[index]->size();
	}
	DEPRECATED NODISCARD constexpr size_type bucket_size(size_type index) const noexcept {
		return m_buckets[index]->size();
	}
	template <class K> NODISCARD size_type bucket(const K& key) const noexcept
		requires(!std::is_convertible_v<K, iterator> && !std::is_convertible_v<K, const_iterator>) {
		return keyToBucket(key);
	}

	NODISCARD constexpr float loadFactor() const noexcept {
		return m_array.size() / bucketCount();
	}
	DEPRECATED NODISCARD constexpr float load_factor() const noexcept {
		return loadFactor();
	}

	template <class K> NODISCARD constexpr bool contains(const K& key) const noexcept {
		auto& bucketList = m_buckets[keyToBucket(key)];
		
		auto found = false;
		for (auto it = bucketList.begin(); it != bucketList.end(); it++) {
			if (m_equal(m_array[*it].first, key)) {
				found = true;
				break;
			}
		}
		return found;
	}
	template <class K> NODISCARD constexpr size_type count(const K& key) const noexcept {
		if (contains(key)) return 1;
		return 0;
	}
	template <class K> NODISCARD constexpr pair_type<iterator, iterator> equalRange(const K& key) const noexcept {
		auto it = find(key);
		return { it, it };
	}
	template <class K> DEPRECATED NODISCARD constexpr pair_type<iterator, iterator> equal_range(const K& key) const noexcept {
		return equalRange(key);
	}

	template <class K> NODISCARD constexpr iterator find(const K& key) noexcept {
		auto& bucketList = m_buckets[keyToBucket(key)];

		for (auto it = bucketList.begin(); it != bucketList.end(); it++) {
			if (m_equal(m_array[*it], key)) return &m_array[*it];
		}

		return m_array.end();
	}
	template <class K> NODISCARD constexpr const_iterator find(const K& key) const noexcept {
		auto& bucketList = m_buckets[keyToBucket(key)];

		for (auto it = bucketList.begin(); it != bucketList.end(); it++)
			if (m_equal(m_array[*it], key)) return &m_array[*it];

		return m_array.end();
	}

	template <class K> NODISCARD constexpr value_type& at(const K& key) {
		auto it = find(key);
		if (it == m_array.end()) throw std::out_of_range("lyra::UnorderedSparseSet::at(): Specified key could not be found in container!");
		return *it;
	}
	template <class K> NODISCARD constexpr const value_type& at(const K& key) const {
		auto it = find(key);
		if (it == m_array.end()) throw std::out_of_range("lyra::UnorderedSparseSet::at(): Specified key could not be found in container!");
		return *it;
	}
	template <class K> NODISCARD constexpr value_type& operator[](const K& key) {
		auto it = find(key);
		return (it == m_array.end()) ? *basicEmplace(value_type()) : *it;
	}
	template <class K> NODISCARD constexpr value_type& operator[](K&& key) {
		auto it = find(std::forward<K>(key));
		return (it == m_array.end()) ? *basicEmplace(value_type()): *it;
	}

private:
	array m_array { };
	buckets m_buckets { };

	NO_UNIQUE_ADDRESS hasher m_hasher { };
	NO_UNIQUE_ADDRESS key_equal m_equal { };

	constexpr void rehashIfNecessary() noexcept {
		if (m_array.size() >= m_buckets.size() * maxLoadFactor) rehash(nextPrime(m_array.size()));
	}
	template <class K> constexpr size_type keyToBucket(const K& key) const noexcept {
		return m_hasher(key) % m_buckets.size();
	}
	constexpr iterator basicInsert(const value_type& value) noexcept {
		auto i = keyToBucket(value);
	
		m_buckets[i].emplaceFront(m_array.size());
		m_array.emplaceBack(value);
		rehashIfNecessary();

		return --m_array.end();
	}
	constexpr iterator basicInsert(value_type&& value) noexcept {
		auto i = keyToBucket(value);

		m_buckets[i].emplaceFront(m_array.size());
		m_array.emplaceBack(std::move(value));
		rehashIfNecessary();

		return --m_array.end();
	}
	template <class K, class... Args> constexpr iterator basicEmplace(K&& key, Args&&... args) noexcept {
		auto i = keyToBucket(std::forward<K>(key));

		m_buckets[i].emplaceFront(m_array.size());
		m_array.emplaceBack(std::forward<K>(key), std::forward<Args>(args)...);
		rehashIfNecessary();

		return --m_array.end();
	}
};

} // namespace lyra
