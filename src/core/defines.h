/*************************
 * @file defines.h
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

#include <math/math.h>

#include <sstream>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>
#include <string>

namespace lyra {

/**
 * @brief get the size of an array
 * 
 * @tparam _Ty type of array
 * @param array the array
 * 
 * @return uint32_t
*/
template<typename _Ty> [[nodiscard]] inline uint32 arr_size(const _Ty* array); 

template<typename _Ty> uint32 arr_size(const _Ty* array) {
	return sizeof(array) / sizeof(array[0]);
}

/**
 * @brief get the address of a pointer
 * 
 * @tparam _Ty type of pointer
 * @param type the pointer
 * 
 * @return std::string 
*/
template<typename _Ty> [[nodiscard]] inline std::string get_address(const _Ty type);

template<typename _Ty> std::string get_address(const _Ty type) {
	const void* address = static_cast<const void*>(type);

	std::stringstream ss; // black magic
	ss << address;

	return std::string ("0x") + ss.str();
}

} // namespace lyra