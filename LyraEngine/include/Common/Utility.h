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

#ifdef _WIN32
// #include <wchar.h>
// #include <Windows.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#endif

#include <vector>
#include <filesystem>

namespace lyra {

/**
 * @brief get the address of a pointer
 * 
 * @tparam Ty type of pointer
 * @param type the pointer
 * 
 * @return const void*
*/
template <typename Ty> NODISCARD constexpr const void* get_address(const Ty& type);

template <typename Ty> constexpr const void* get_address(const Ty& type) {
	return static_cast<const void*>(type);
}

} // namespace lyra
