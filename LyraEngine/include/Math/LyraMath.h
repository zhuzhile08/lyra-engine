/*************************
 * @file LyraMath.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief some mathematical functions
 * 
 * @date 2022-02-03
 * 
 * @copyright Copyright (c) 2022
 *************************/

#pragma once

#include <Common/Common.h>

#include <cmath>
#include <LSD/Vector.h>
#include <glm/glm.hpp>

namespace lyra {

template <size_type Size, class Ty> NODISCARD constexpr Ty pythagoras(const glm::vec<Size, Ty, glm::defaultp>& a, const glm::vec<Size, Ty, glm::defaultp>& b);

template <size_type Size, class Ty> constexpr Ty pythagoras(const glm::vec<Size, Ty, glm::defaultp>& a, const glm::vec<Size, Ty, glm::defaultp>& b) {
	Ty result;
	for (uint8 x = 0; x < Size; x++) {
		result += pow(a[x] - b[x], 2);
	}
	return sqrt(result);
}

template <class Ty> NODISCARD constexpr Ty point_on_line(Ty first, Ty second, float32 value);

template <class Ty> constexpr Ty point_on_line(Ty first, Ty second, float32 value) {
	return first + (second - first) * value;
}

template <class Ty> NODISCARD constexpr Ty bezier(lsd::Vector<Ty> points, float32 value);

template <class Ty> constexpr Ty bezier(lsd::Vector<Ty> points, float32 value) {
	lsd::Vector<Ty> remaining_points;

	for (uint32 i = 0; i <= points.size(); i++) remaining_points.pushBack(point_on_line<Ty>(points.at(i), points.at(i + 1), value));
	
	if (remaining_points.size() == 2) return point_on_line<Ty>(points.at(0), points.at(1), value);

	bezier<Ty>(remaining_points, value);
}

NODISCARD float32 randfloat32(float32 x, float32 y);

void decompose_transform_matrix(
	const glm::mat4& matrix, 
	glm::vec3& translation, 
	glm::vec3& rotation, 
	glm::vec3& scale, 
	glm::vec3& forward, 
	glm::vec3& up, 
	glm::vec3& left
);

} // namespace lyra
