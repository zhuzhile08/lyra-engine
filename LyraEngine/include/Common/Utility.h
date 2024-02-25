/*************************
 * @file Utility.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief miscelanious functions
 * 
 * @date 2022-02-03
 * 
 * @copyright Copyright (c) 2022
 * 
 *************************/

#pragma once

#include <Common/Common.h>

#include <functional>
#include <string>
#include <string_view>

namespace lyra {

template <typename Ty> NODISCARD constexpr const void* getAddress(const Ty& type);

template <typename Ty> constexpr const void* getAddress(const Ty& type) {
	return static_cast<const void*>(type);
}

inline Vector<std::string> parse(std::string_view s, std::string_view d) {
	Vector<std::string> r;

	size_t begin = 0;
	size_t current;

	while ((current = s.find(d, begin)) != std::string::npos) {
		r.emplaceBack(s.substr(begin, current - begin));
		begin = current + d.size();
	}
	
	r.emplaceBack(s.substr(begin));

	return r;
}

inline Vector<std::wstring> parse(std::wstring_view s, std::wstring_view d) {
	Vector<std::wstring> r;

	size_t begin = 0;
	size_t current;

	while ((current = s.find(d, begin)) != std::string::npos) {
		r.emplaceBack(s.substr(begin, current - begin));
		begin = current + d.size();
	}
	
	r.emplaceBack(s.substr(begin));

	return r;
}


template <class Ty> struct CallableUnderlying {
	using type = Ty;
};

template <class Ty, class... Args> struct CallableUnderlying<Ty(Args...)> {
	typedef Ty(*type)(Args...);
};

template <class Ty> concept EnumType = std::is_enum_v<Ty>;

// credits to https://gist.github.com/StrikerX3/46b9058d6c61387b3f361ef9d7e00cd4 for these operators!

template<EnumType Enum> constexpr inline Enum operator|(Enum first, Enum second) {
	return static_cast<Enum>(
		static_cast<std::underlying_type_t<Enum>>(first) |
		static_cast<std::underlying_type_t<Enum>>(second)
	);
}

template<EnumType Enum> Enum constexpr inline operator&(Enum first, Enum second) {
	return static_cast<Enum>(
		static_cast<std::underlying_type_t<Enum>>(first) &
		static_cast<std::underlying_type_t<Enum>>(second)
	);
}

template<EnumType Enum> Enum constexpr inline operator^(Enum first, Enum second) {
	return static_cast<Enum>(
		static_cast<std::underlying_type_t<Enum>>(first) ^
		static_cast<std::underlying_type_t<Enum>>(second)
	);
}

template<EnumType Enum> Enum constexpr inline operator~(Enum first) {
	return static_cast<Enum>(
		~static_cast<std::underlying_type_t<Enum>>(first)
	);
}

template<EnumType Enum> Enum constexpr inline operator|=(Enum& first, Enum second) {
	return (first = static_cast<Enum>(
		static_cast<std::underlying_type_t<Enum>>(first) |
		static_cast<std::underlying_type_t<Enum>>(second)
	));
}

template<EnumType Enum> Enum constexpr inline operator&=(Enum& first, Enum second) {
	return (first = static_cast<Enum>(
		static_cast<std::underlying_type_t<Enum>>(first) &
		static_cast<std::underlying_type_t<Enum>>(second)
	));
}

template<EnumType Enum> Enum constexpr inline operator^=(Enum& first, Enum second) {
	return (first = static_cast<Enum>(
		static_cast<std::underlying_type_t<Enum>>(first) ^
		static_cast<std::underlying_type_t<Enum>>(second)
	));
}

} // namespace lyra

namespace std {

template<lyra::EnumType Enum> constexpr inline std::string to_string(Enum e) {
	return std::to_string(static_cast<std::underlying_type_t<Enum>>(e));
}

template<lyra::EnumType Enum> struct hash<Enum> {
	std::size_t operator()(Enum t) const noexcept {
		return static_cast<std::size_t>(t);
	}
};

}
