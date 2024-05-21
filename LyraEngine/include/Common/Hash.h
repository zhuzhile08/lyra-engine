/*************************
 * @file Hash.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Common hash function implementations
 * 
 * @date 2024-03-02
 * 
 * @copyright Copyright (c) 2024
 *************************/

#pragma once

#include <Common/Common.h>
#include <Common/Utility.h>

#include <type_traits>
#include <typeindex>
#include <functional>
#include <filesystem>

namespace lyra {

template <class Ty> concept IntegralType = std::is_integral_v<Ty>;
template <class Ty> concept PointerType = std::is_pointer_v<Ty>;


template<IntegralType Integral> struct Hash<Integral> {
	constexpr size_type operator()(Integral i) const noexcept {
		if constexpr (sizeof(i) <= sizeof(size_type)) {
			return static_cast<size_type>(i);
		} else {
			i = (i ^ (i >> 30)) * Integral(0xbf58476d1ce4e5b9);
			i = (i ^ (i >> 27)) * Integral(0x94d049bb133111eb);
			return i ^ (i >> 31);
		}
	}
};

template<PointerType Pointer> struct Hash<Pointer> {
	constexpr size_type operator()(Pointer p) const noexcept {
		auto i = reinterpret_cast<size_type>(p);
		i = (i ^ (i >> 30)) * size_type(0xbf58476d1ce4e5b9);
		i = (i ^ (i >> 27)) * size_type(0x94d049bb133111eb);
		return i ^ (i >> 31);
	}
};

template<> struct Hash<std::nullptr_t> {
	constexpr size_type operator()(std::nullptr_t) const noexcept {
		return 0;
	}
};

template<EnumType Enum> struct Hash<Enum> {
	constexpr size_type operator()(Enum e) const noexcept {
		return static_cast<size_type>(e);
	}
};

template <> struct Hash<std::type_index> {
	size_type operator()(std::type_index t) const noexcept {
		return t.hash_code();
	}
};

template <class C> struct Hash<std::basic_string<C>> {
	using string_type = std::basic_string<C>;

	size_type operator()(const string_type& s) const noexcept {
		return std::hash<string_type>{}(s); /// @todo implement own hash function
	}
};

template <> struct Hash<std::filesystem::path> {
	size_type operator()(const std::filesystem::path& p) const noexcept {
		return std::filesystem::hash_value(p); /// @todo implement own hash function
	}
};

} // namespace lyra

#define CUSTOM_HASHER(name, type, hashType, hasher, toHashType)\
class name {\
public:\
	constexpr size_type operator()(type ty) const noexcept {\
		return hasher((ty)toHashType);\
	}\
	constexpr size_type operator()(hashType hash) const noexcept {\
		return hasher(hash);\
	}\
};

#define CUSTOM_EQUAL(name, type, hashType, toHashType)\
class name {\
public:\
	constexpr bool operator()(type first, type second) const noexcept {\
		return (first)toHashType == (second)toHashType;\
	}\
	constexpr bool operator()(type first, hashType second) const noexcept {\
		return (first)toHashType == second;\
	}\
	constexpr bool operator()(hashType first, type second) const noexcept {\
		return first == (second)toHashType;\
	}\
	constexpr bool operator()(hashType first, hashType second) const noexcept {\
		return first == second;\
	}\
};
