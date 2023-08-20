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

template <class Ty> concept EnumHashType = std::is_enum_v<Ty>;

class EnumHash {
public:
	template <EnumHashType Ty> size_t operator()(Ty t) const {
		return static_cast<size_t>(t);
	}
};

} // namespace lyra
