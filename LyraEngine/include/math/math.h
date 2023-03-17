/*************************
 * @file math.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief some mathematical functions
 * 
 * @date 2022-02-03
 * 
 * @copyright Copyright (c) 2022
 * 
 *************************/

#pragma once

#include <lyra.h>

#include <cmath>
#include <vector>
#include <glm/glm.hpp>

namespace lyra {

/**
 * @brief an implementation of the pythagorean theorum
 * 
 * @tparam _Size size of the vectors
 * @tparam Ty type stored in the vectors
 * @param a first vector
 * @param b second vector
 * 
 * @return Ty 
 */
template <size_t Size, class Ty> NODISCARD constexpr Ty pythagoras(const glm::vec<Size, Ty, glm::defaultp>& a, const glm::vec<Size, Ty, glm::defaultp>& b);

template <size_t Size, class Ty> constexpr Ty pythagoras(const glm::vec<Size, Ty, glm::defaultp>& a, const glm::vec<Size, Ty, glm::defaultp>& b) {
	Ty result;
	for (uint8 x = 0; x < Size; x++) {
		result += pow(a[x] - b[x], 2);
	}
	return sqrt(result);
}

/**
 * @brief get the position on a line based on a normalized value
 * 
 * @tparam type of vector to use
 * 
 * @param first first point
 * @param second second point
 * @param value normalized length
 * 
 * @return Ty
*/
template <class Ty> NODISCARD constexpr Ty point_on_line(Ty first, Ty second, float value);

template <class Ty> constexpr Ty point_on_line(Ty first, Ty second, float value) {
	return first + (second - first) * value;
}

/**
 * @brief get a point on a bezier curve
 * 
 * @tparam Ty type of the vector
 * 
 * @param points points of the bezier curve
 * @param value normalized position on the bezier
 * 
 * @return Ty 
*/
template <class Ty> NODISCARD constexpr Ty bezier(std::vector<Ty> points, float value);

template <class Ty> constexpr Ty bezier(std::vector<Ty> points, float value) {
	std::vector<Ty> remaining_points;

	for (uint32 i = 0; i <= points.size(); i++) remaining_points.push_back(point_on_line<Ty>(points.at(i), points.at(i + 1), value));
	
	if (remaining_points.size() == 2) return point_on_line<Ty>(points.at(0), points.at(1), value);

	bezier<Ty>(remaining_points, value);
}

/**
 * @brief randomly generate a floating point number
 * 
 * @param x bottom limit
 * @param y upper limit
 * 
 * @return float
 */
NODISCARD float randFloat(const float x, const float y);

/**
 * @brief decompose the transformation matrix
 * 
 * @param matrix matrix to decompose
 * @param translation translation vector
 * @param rotation rotation vector
 * @param scale scale vector
 */
void decompose_transform_matrix(
	const glm::mat4& matrix, 
	glm::vec3& translation, 
	glm::vec3& rotation, 
	glm::vec3& scale, 
	glm::vec3& forward, 
	glm::vec3& up, 
	glm::vec3& left
);

// pointer alignment mode
enum AlignMode : uint8 {
	ALIGN_FORWARD,
	ALIGH_BACKWARD
};

/**
 * @brief align a address to a memory aligment
 *
 * @param address address
 * @param alignment alignment
 * @param mode align mode
 */
void alignPointer(void* address, const uint8 alignment, const uint8 mode = 0);

/**
 * @brief calculate by how many bytes the address has to be shifted to be aligned
 *
 * @param address address
 * @param alignment alignment
 * @param mode align mode
 *
 * @return const uint8
 */
NODISCARD const uint8 alignPointerAdjustment(const void* address, const uint8 alignment, const uint8 mode = 0);

} // namespace lyra
