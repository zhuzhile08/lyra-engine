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

#include <math/math.h>

#include <core/decl.h>

#include <sstream>
#include <string>
#include <vector>
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
template<typename _Ty> [[nodiscard]] inline const void* get_address(const _Ty type);

template<typename _Ty> const void* get_address(const _Ty type) {
	return static_cast<const void*>(type);
}

/**
 * @brief move a element of a vector to another
 * 
 * @tparam _Ty type of vector
 * @param src source vector
 * @param dst destination vector
 * @param index index of the element
 */
template<typename _Ty> void move_element(std::vector<_Ty>& src, std::vector<_Ty>& dst, int index);

template<typename _Ty> void move_element(std::vector<_Ty>& src, std::vector<_Ty>& dst, int index) {
	dst.push_back(std::move(src.at(index)));
	src.erase(src.begin() + index);
}

#define FUNC_PTR(func) [&] { func }

} // namespace lyra