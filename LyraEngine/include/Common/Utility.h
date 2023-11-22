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

namespace lyra {

template <typename Ty> NODISCARD constexpr const void* getAddress(const Ty& type);

template <typename Ty> constexpr const void* getAddress(const Ty& type) {
	return static_cast<const void*>(type);
}

template <class Ty> concept EnumType = std::is_enum_v<Ty>;

class EnumHash {
public:
	template <EnumType Ty> size_t operator()(Ty t) const {
		return static_cast<size_t>(t);
	}
};

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

template<lyra::EnumType Enum> constexpr inline string to_string(Enum e) {
	return to_string(static_cast<std::underlying_type_t<Enum>>(e));
}

};
