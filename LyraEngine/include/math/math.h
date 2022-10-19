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
 */

#pragma once

#include <vector>
#include <glm.hpp>

namespace lyra {

/**
 * @brief an implementation of pythagoras theorem
 * 
 * @param a the first vector
 * @param b the second vector
 * @cond the placement of the vectors don't matter
 * 
 * @return double 
 */
NODISCARD float pyth(const glm::vec2 a, const glm::vec2 b);

/**
 * @brief an implementation of pythagoras theorem, but in 3D
 * 
 * @param a the first vector
 * @param b the second vector
 * 
 * @return double 
 */
NODISCARD float pyth3(const glm::vec3 a, const glm::vec3 b);

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
template<class Ty> NODISCARD Ty point_on_line(Ty first, Ty second, float value);

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
template<class Ty> NODISCARD Ty bezier(std::vector<Ty> points, float value);

/**
 * @brief randomly generate a floating point number
 * 
 * @param x bottom limit
 * @param y upper limit
 * 
 * @return float
 */
NODISCARD float randFloat(const float x, const float y);

// pointer alignment mode
enum AlignMode : uint8_t {
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
void alignPointer(void* address, const uint8_t alignment, const uint8_t mode = 0);

/**
 * @brief calculate by how many bytes the address has to be shifted to be aligned
 *
 * @param address address
 * @param alignment alignment
 * @param mode align mode
 *
 * @return const uint8_t
 */
NODISCARD const uint8_t alignPointerAdjustment(const void* address, const uint8_t alignment, const uint8_t mode = 0);

} // namespace lyra
