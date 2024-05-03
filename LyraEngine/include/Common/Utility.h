/*************************
 * @file Utility.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief miscelanious functions
 * 
 * @date 2022-02-03
 * 
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <Common/Common.h>

#include <functional>
#include <string>
#include <string_view>

namespace lyra {

template <typename Ty> NODISCARD constexpr inline const void* getAddress(const Ty& type) noexcept {
	return static_cast<const void*>(type);
}

template <template <class...> class Container> NODISCARD inline constexpr Container<std::string> parse(std::string_view s, std::string_view d) noexcept {
	Container<std::string> r;

	size_type begin = 0;
	size_type current;

	while ((current = s.find(d, begin)) != std::string::npos) {
		r.emplace_back(s.substr(begin, current - begin));
		begin = current + d.size();
	}
	
	r.emplace_back(s.substr(begin));

	return r;
}

template <template <class...> class Container> NODISCARD inline constexpr Container<std::wstring> parse(std::wstring_view s, std::wstring_view d) noexcept {
	Container<std::wstring> r;

	size_type begin = 0;
	size_type current;

	while ((current = s.find(d, begin)) != std::string::npos) {
		r.emplace_back(s.substr(begin, current - begin));
		begin = current + d.size();
	}
	
	r.emplace_back(s.substr(begin));

	return r;
}


// prime number utility
template <class Integer> inline constexpr bool isPrime(Integer n) noexcept requires std::is_integral_v<Integer> {
	if (n == 2 || n == 3)
		return true;
	if (n <= 1 || n % 2 == 0 || n % 3 == 0)
		return false;
	for (Integer i = 5; i * i <= n; i += 6)
		if (n % i == 0 || n % (i + 2) == 0)
			return false;
	return true;
};

template <class Integer> inline constexpr Integer nextPrime(Integer n) noexcept requires std::is_integral_v<Integer> {
	if (n % 2 == 0)
		--n;

	while (true) {
		n += 2;
		if (isPrime(n)) {
			return n;
		}
	}
};

template <class Integer> inline constexpr Integer lastPrime(Integer n) noexcept requires std::is_integral_v<Integer> {
	if (n % 2 == 0)
		++n;

	while (true) {
		n -= 2;
		if (isPrime(n)) {
			return n;
		}
	}
};

template <class Integer> inline constexpr Integer sizeToIndex(Integer size) noexcept requires std::is_integral_v<Integer> {
	return (size == 0) ? 0 : size - 1;
}

template <class Ty> concept EnumType = std::is_enum_v<Ty>;

// credits to https://gist.github.com/StrikerX3/46b9058d6c61387b3f361ef9d7e00cd4 for these operators!

template<EnumType Enum> constexpr inline Enum operator|(Enum first, Enum second) noexcept {
	return static_cast<Enum>(
		static_cast<std::underlying_type_t<Enum>>(first) |
		static_cast<std::underlying_type_t<Enum>>(second)
	);
}

template<EnumType Enum> Enum constexpr inline operator&(Enum first, Enum second) noexcept {
	return static_cast<Enum>(
		static_cast<std::underlying_type_t<Enum>>(first) &
		static_cast<std::underlying_type_t<Enum>>(second)
	);
}

template<EnumType Enum> Enum constexpr inline operator^(Enum first, Enum second) noexcept {
	return static_cast<Enum>(
		static_cast<std::underlying_type_t<Enum>>(first) ^
		static_cast<std::underlying_type_t<Enum>>(second)
	);
}

template<EnumType Enum> Enum constexpr inline operator~(Enum first) noexcept {
	return static_cast<Enum>(
		~static_cast<std::underlying_type_t<Enum>>(first)
	);
}

template<EnumType Enum> Enum constexpr inline operator|=(Enum& first, Enum second) noexcept {
	return (first = static_cast<Enum>(
		static_cast<std::underlying_type_t<Enum>>(first) |
		static_cast<std::underlying_type_t<Enum>>(second)
	));
}

template<EnumType Enum> Enum constexpr inline operator&=(Enum& first, Enum second) noexcept {
	return (first = static_cast<Enum>(
		static_cast<std::underlying_type_t<Enum>>(first) &
		static_cast<std::underlying_type_t<Enum>>(second)
	));
}

template<EnumType Enum> Enum constexpr inline operator^=(Enum& first, Enum second) noexcept {
	return (first = static_cast<Enum>(
		static_cast<std::underlying_type_t<Enum>>(first) ^
		static_cast<std::underlying_type_t<Enum>>(second)
	));
}

} // namespace lyra

namespace std {

template<lyra::EnumType Enum> constexpr inline std::string to_string(Enum e) noexcept {
	return std::to_string(static_cast<std::underlying_type_t<Enum>>(e));
}

}
