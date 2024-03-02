/*************************
 * @file SparseSet.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Sparse Set implementation
 * 
 * @date 2024-02-24
 * 
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <Common/Common.h>
#include <Common/Utility.h>
#include <Common/Iterator.h>

#include <initializer_list>
#include <functional>
#include <utility>

namespace lyra {

template <
	class Key,
	class Hash = std::hash<Key>,
	class Equal = std::equal_to<Key>
> class SparseSet {
public:
	static constexpr float maxLoadFactor = 2;

	using key_type = Key;
	using size_type = size_t;
	template <class F, class S> using pair_type = std::pair<F, S>;

	using value_type = key_type;
	using reference = value_type&;
	using const_reference = const value_type&;
	using rvreference = value_type&&;
	using pointer = value_type*;
	using const_pointer = const pointer;
	using array = Vector<value_type>;

	using bucket_type = pair_type<size_type, key_type>;
	using bucket_list = Vector<bucket_type>;
	using buckets = Vector<bucket_list>;

	using lookup_type = pair_type<size_type, size_type>;
	using lookup = Vector<lookup_type>;

	using iterator = typename array::iterator;
	using const_iterator = typename array::const_iterator;
	using bucket_iterator = typename buckets::iterator;
	using const_bucket_iterator = typename buckets::const_iterator;

	using hasher = Hash;
	using equal = Equal;

	using wrapper = SparseSet;

	constexpr SparseSet() : m_buckets(2) { } 
	constexpr SparseSet(size_type bucketCount) : m_buckets(std::max(bucketCount, size_type(2))) { } 
	template <class It> constexpr SparseSet(It first, It last, size_type bucketCount = 0) : m_buckets(bucketCount) {  // set to 0 for default evaluation
		insert(first, last);
	}
	constexpr SparseSet(std::initializer_list<value_type> ilist, size_type bucketCount = 0) : m_buckets(bucketCount) { // set to 0 for default evaluation
		insert(ilist.begin(), ilist.end());
	} 
	constexpr SparseSet(const SparseSet& other) : m_array(other.m_array), m_buckets(other.m_buckets), m_lookup(other.m_lookup) { }
	constexpr SparseSet(SparseSet&& other) : m_array(std::move(other.m_array)), m_buckets(std::move(other.m_buckets)), m_lookup(std::move(other.m_lookup)) { }

	constexpr void swap(wrapper& other) {
		m_array.swap(other.m_array);
		m_buckets.swap(other.m_buckets);
		m_lookup.swap(other.m_lookup);
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
	constexpr const_iterator rcbegin() const noexcept {
		return m_array.rcbegin();
	}
	constexpr iterator rend() noexcept {
		return m_array.rend();
	}
	constexpr const_iterator rend() const noexcept {
		return m_array.rend();
	}
	constexpr const_iterator rcend() const noexcept {
		return m_array.rcend();
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
	constexpr const_bucket_iterator rcbegin(size_type index) const noexcept {
		return m_buckets[index].rcbegin();
	}
	constexpr bucket_iterator rend(size_type index) noexcept {
		return m_buckets[index].rend();
	}
	constexpr const_bucket_iterator rend(size_type index) const noexcept {
		return m_buckets[index].rend();
	}
	constexpr const_bucket_iterator rcend(size_type index) const noexcept {
		return m_buckets[index].rcend();
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
		m_lookup.resize(m_array.size());

		for (size_type i = 0; i < m_array.size(); i++) {
			auto index = keyToBucket(m_array[i]);

			m_lookup[i] = { index, m_buckets[index].size() };
			m_buckets[index].emplaceBack(i, m_array[i]);
		}
	}

	constexpr pair_type<iterator, bool> insert(const_reference value) noexcept {
		auto it = basicFind(value);

		if (it != m_array.end())
			return { it, false };
		else
			return { basicInsert(value), true };
	}
	template <class Value> constexpr pair_type<iterator, bool> insert(Value&& value) noexcept {
		return emplace(std::forward<Value>(value));
	}
	DEPRECATED constexpr iterator insert(const_bucket_iterator hint, const_reference value) noexcept {
		return insert(value).first;
	}
	template <class Value> DEPRECATED constexpr iterator insert(const_bucket_iterator hint, Value&& value) noexcept {
		return insert(std::forward<Value>(value)).first;
	}
	template <class It> constexpr void insert(It first, It last) noexcept {
		if (first != last) {
			size_type newSize = m_array.size() + (last - first);
			m_array.reserve(newSize);
			m_lookup.reserve(newSize);

			for (; first != last; first++) {
				basicInsert(*first);
			}
		}
	}
	constexpr void insert(std::initializer_list<value_type> ilist) noexcept {
		insert(ilist.begin(), ilist.end());
	}

	template <class K, class V> constexpr pair_type<iterator, bool> insertOrAssign(K&& key) noexcept {
		auto it = basicFind(key);

		if (it != m_array.end()) {
			*it = std::forward<K>(key);
			return { it, false };
		} else {
			return { basicEmplace(std::forward<K>(key)), true };
		}
	}
	template <class K, class V> DEPRECATED constexpr pair_type<iterator, bool> insert_or_assign(K&& key) noexcept {
		return insertOrAssign(std::forward<K>(key));
	}
	template <class K, class V> DEPRECATED constexpr pair_type<iterator, bool> insertOrAssign(const_iterator hint, K&& key) noexcept {
		return insertOrAssign(std::forward<K>(key));
	}
	template <class K, class V> DEPRECATED constexpr pair_type<iterator, bool> insert_or_assign(const_iterator hint, K&& key) noexcept {
		return insertOrAssign(std::forward<K>(key));
	}

	template <class K, class... Args> constexpr pair_type<iterator, bool> tryEmplace(K&& key) noexcept {
		auto it = basicFind(std::forward<K>(key));

		if (it != m_array.end()) {
			return { it, false };
		} else {
			return { basicEmplace(std::forward<K>(key)), true };
		}
	}
	template <class K, class... Args> DEPRECATED constexpr iterator tryEmplace(const_iterator hint, K&& key) noexcept {
		auto it = basicFind(std::forward<K>(key));

		if (it != m_array.end()) {
			return it;
		} else {
			return basicEmplace(std::forward<K>(key));
		}
	}
	template <class K, class... Args> DEPRECATED constexpr iterator try_emplace(const_iterator hint, K&& key) noexcept {
		tryEmplace(hint, std::forward<K>(key));
	}

	template <class... Args> constexpr pair_type<iterator, bool> emplace(Args&&... args) noexcept {
		value_type value(std::forward<Args>(args)...);
		auto it = basicFind(value.first);

		if (it != m_array.end()) {
			return { it, false };
		} else {
			return { basicInsert(std::move(value)), true };
		}
	}
	template <class... Args> DEPRECATED constexpr iterator emplaceHint(const_iterator hint, Args&&... args) noexcept {
		return emplace(std::forward<Args>(args)...);
	}
	template <class... Args> DEPRECATED constexpr iterator emplace_hint(const_iterator hint, Args&&... args) noexcept {
		return emplace(std::forward<Args>(args)...);
	}

	constexpr iterator erase(iterator pos) noexcept {
		ASSERT(pos != m_array.end(), "lyra::SparseSet::erase(): The end iterator was passed to the function!");

		auto index = pos - m_array.begin();
		
		m_buckets.erase(m_buckets.begin() + index);
		m_lookup.erase(m_lookup.begin() + index);
		for (; index < m_lookup.size();) {
			auto& elemLookup = m_lookup[index++];
			--(m_buckets[elemLookup.first][elemLookup.second].first);
		}

		return m_array.erase(pos);
	}
	constexpr iterator erase(const_iterator pos) noexcept {
		ASSERT(pos != em_array.nd(), "lyra::SparseSet::erase(): The end iterator was passed to the function!");

		auto index = pos - m_array.begin();

		m_buckets.erase(m_buckets.begin() + index);
		m_lookup.erase(m_lookup.begin() + index);
		for (; index < m_lookup.size();) {
			auto& elemLookup = m_lookup[index++];
			--(m_buckets[elemLookup.first][elemLookup.second].first);
		}

		return m_array.erase(pos);
	}
	constexpr iterator erase(const_iterator first, const_iterator last) noexcept {
		if (first != last) {
			for (iterator it = first; it != last; it++) {
				auto& lookup = m_lookup[it - m_array.begin()];
				auto& bucketList = m_buckets[lookup.first];
				
				bucketList.erase(lookup.second);
				for (size_type i = 0; i < bucketList.size(); i++) --(bucketList[i].first);
			}

			auto lookupFirst = m_lookup.begin() + (first - m_array.begin());
			m_lookup.erase(lookupFirst, lookupFirst + (last - first));
			return m_array.erase(first, last);
		}

		return m_array.end();
	}
	template <class K> constexpr size_type erase(K&& key) noexcept {
		auto it = basicFind(std::forward<K>(key));

		if (it != m_array.end()) {
			erase(it);
			return 1;
		}

		return 0;
	}

	constexpr value_type extract(const_iterator pos) noexcept {
		if (pos != m_array.end()) {
			value_type v = std::move(*pos);
			erase(pos);
			return v;
		}

		return value_type();
	}
	template <class K> constexpr value_type extract(K&& key) noexcept {
		return extract(find(std::forward<K>(key)));
	} 

	constexpr void clear() noexcept {
		m_array.clear();
		m_buckets.clear();
	}

	NODISCARD constexpr size_type size() const noexcept {
		return m_array.size();
	}
	NODISCARD constexpr size_type maxSize() const noexcept {
		return m_array.max_size();
	}
	DEPRECATED NODISCARD constexpr size_type max_size() const noexcept {
		return maxSize();
	}
	NODISCARD bool empty() const noexcept {
		return m_array.empty();
	}

	NODISCARD constexpr size_type bucketCount() const noexcept {
		return m_buckets.size();
	}
	DEPRECATED NODISCARD constexpr size_type bucket_count() const noexcept {
		return m_buckets.size();
	}
	NODISCARD constexpr size_type maxBucketSize() const noexcept {
		return m_buckets.max_size();
	}
	DEPRECATED NODISCARD constexpr size_type max_bucket_size() const noexcept {
		return m_buckets.max_size();
	}
	NODISCARD constexpr size_type bucketSize(size_type index) const noexcept {
		return m_buckets[index].size();
	}
	DEPRECATED NODISCARD constexpr size_type bucket_size(size_type index) const noexcept {
		return m_buckets[index].size();
	}
	template <class K> NODISCARD size_type bucket(const K& key) const noexcept {
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
		
        for (size_type i = 0; i < bucketList.size(); i++)
            return (m_equal(bucketList[i].second, key));
	}
	template <class K> NODISCARD constexpr size_type count(const K& key) const noexcept {
		if (contains(key)) return 1;
		return 0;
	}
	template <class K> NODISCARD constexpr pair_type<iterator, iterator> equalRange(const K& key) const noexcept {
		auto it = basicFind(key);
		return { it, it };
	}
	template <class K> DEPRECATED NODISCARD constexpr pair_type<iterator, iterator> equal_range(const K& key) const noexcept {
		return equalRange(key);
	}

	template <class K> NODISCARD constexpr iterator find(const K& key) noexcept {
		return basicFind(key);
	}
	template <class K> NODISCARD constexpr const_iterator find(const K& key) const noexcept {
		return basicFind(key);
	}

	template <class K> NODISCARD constexpr value_type& at(const K& key) {
		auto it = basicFind(key);
		if (it == end()) throw std::out_of_range("lyra::SparseSet::at(): Specified key could not be found in container!");
		return it->second;
	}
	template <class K> NODISCARD constexpr const value_type& at(const K& key) const {
		auto it = basicFind(key);
		if (it == end()) throw std::out_of_range("lyra::SparseSet::at(): Specified key could not be found in container!");
		return it->second;
	}
	template <class K> NODISCARD constexpr value_type& operator[](const K& key) {
		auto it = basicFind(key);
		return (it != m_array.end()) ? *it : *basicInsert(key);
	}
	template <class K> NODISCARD constexpr value_type& operator[](K&& key) {
		auto it = basicFind(std::forward<K>(key));
		return (it != m_array.end()) ? *it : *basicEmplace(std::forward<K>(key));
	}

private:
	array m_array { };
	lookup m_lookup { };
	buckets m_buckets { };

	NO_UNIQUE_ADDRESS hasher m_hasher { };
	NO_UNIQUE_ADDRESS equal m_equal { };

	constexpr void rehashIfNecessary() noexcept {
		if (m_array.size() >= m_buckets.size() * maxLoadFactor) rehash(nextPrime(m_array.size()));
	}
	template <class K> constexpr size_type keyToBucket(const K& key) noexcept {
		return m_hasher(key) % m_buckets.size();
	}
	template <class K> constexpr iterator basicFind(const K& key) noexcept {
		auto& bucketList = m_buckets[keyToBucket(key)];

		for (auto it = bucketList.begin(); it != bucketList.end(); it++) 
			if (m_equal(it->second, key)) return &m_array[it->first];

		return m_array.end();
	}
	constexpr iterator basicInsert(const value_type& value) noexcept {
		auto i = keyToBucket(value);

		m_lookup.emplaceBack(i, m_buckets[i].size());
		m_buckets[i].emplaceBack(m_array.size(), value);
		iterator it = &m_array.emplaceBack(std::move(value));
		rehashIfNecessary();

		return *it;
	}
	template <class K, class... Args> constexpr iterator basicEmplace(K&& key) noexcept {
		auto i = keyToBucket(std::forward<K>(key));

		m_lookup.emplaceBack(i, m_buckets[i].size());
		m_buckets[i].emplaceBack(m_array.size(), std::forward<K>(key));
		iterator it = &m_array.emplaceBack(std::forward<K>(key));

		rehashIfNecessary();

		return *it;
	}
};

} // namespace lyra
