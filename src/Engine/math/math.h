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

#include <math.h>
#include <chrono>
#include <glm.hpp>

namespace lyra {

// some numbers
#define EULER 2.718281828459045
#define PI 3.141592653589793
#define GOLDENR 1.618033988749894

// abbreviations for fixed width integers
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

/**
 * @brief an implementation of pythagoras theorem
 * 
 * @param a the first vector
 * @param b the second vector
 * @cond the placement of the vectors don't matter
 * @return double 
 */
double pyth(const glm::vec2 a, const glm::vec2 b);

/**
 * @brief an implementation of pythagoras theorem, but in 3D
 * 
 * @param a the first vector
 * @param b the second vector
 * 
 * @return double 
 */
double pyth3(const glm::vec3 a, const glm::vec3 b);

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
template<class Ty> Ty point_on_line(Ty first, Ty second, float value);

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
template<class Ty> Ty bezier(std::vector<Ty> points, float value);

/**
 * @brief calculate the deltatime from the current FPS
 *
 * @return double
 */
const float FPS();
/**
 * @brief calculate the deltatime from the current FPS
 * 
 * @return double 
 */
const double deltaTime(double FPS);

/**
 * @brief randomly generate a floating point number
 * 
 * @param x bottom limit
 * @param y upper limit
 * @return double 
 */
double randDoub(const double x, const double y);

} // namespace lyra
