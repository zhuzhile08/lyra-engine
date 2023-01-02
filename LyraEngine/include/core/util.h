/*************************
 * @file util.h
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

#include <lyra.h>

#ifdef _WIN32
#include <wchar.h>
#include <Windows.h>
#elif __APPLE__
#include <mach-o/dyld.h>
#endif

#include <vector>
#include <filesystem>

namespace lyra {

/**
 * @brief get the size of an array
 * 
 * @tparam Ty type of array
 * @param array the array
 * 
 * @return constexpr uint32_t
*/
template <typename Ty> NODISCARD constexpr uint32 arr_size(const Ty* array); 

template <typename Ty> constexpr uint32 arr_size(const Ty* array) {
	// return sizeof(array) / sizeof(array[0]);
	return static_cast<uint32>(*(&array + 1) - array); // if you won't take my normal and understandable code, take this instead!
}

/**
 * @brief get the address of a pointer
 * 
 * @tparam Ty type of pointer
 * @param type the pointer
 * 
 * @return constexpr const void*
*/
template <typename Ty> NODISCARD constexpr const void* get_address(const Ty type);

template <typename Ty> constexpr const void* get_address(const Ty type) {
	return static_cast<const void*>(type);
}

/**
 * @brief move a element of a vector to another
 * 
 * @tparam Ty type of vector
 * @param src source vector
 * @param dst destination vector
 * @param index index of the element
 */
template <typename Ty> inline void move_element(std::vector<Ty>& src, std::vector<Ty>& dst, int index);

template <typename Ty> void move_element(std::vector<Ty>& src, std::vector<Ty>& dst, int index) {
	dst.push_back(std::move(src.at(index)));
	src.erase(src.begin() + index);
}

/**
 * @brief platform agnostic function to get the current path of the executable
 * 
 * @return std::filesystem::path 
 */
NODISCARD inline std::filesystem::path get_executable_path();

std::filesystem::path get_executable_path() {
#ifdef __APPLE__
	char buffer [PATH_MAX];
	uint32_t bufsize = PATH_MAX;
	if (!_NSGetExecutablePath(buffer, &bufsize))
		puts(buffer);
	return std::filesystem::path(buffer).parent_path();
#elif linux
	std::filesystem::path path("/proc/self/exe");
	std::filesystem::canonical(path);
	return path;
#endif
}

} // namespace lyra
