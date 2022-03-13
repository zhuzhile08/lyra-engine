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
#include <glm.hpp>

namespace lyra {

// some numbers
#define EULER   2.718281828459045
#define PI      3.141592653589793
#define GOLDENR 1.618033988749894

// abbreviations for fixed width integers
typedef uint8_t     uint8;
typedef uint16_t    uint16;
typedef uint32_t    uint32;
typedef uint64_t    uint64;

typedef int8_t      int8;
typedef int16_t     int16;
typedef int32_t     int32;
typedef int64_t     int64;

/**
 * @brief an implementation of pythagoras theorem
 * 
 * @param a the first vector
 * @param b the second vector
 * @cond the placement of the vectors don't matter
 * @return double 
 */
double pyth(glm::vec2 a, glm::vec2 b);

/**
 * @brief an implementation of pythagoras theorem, but in 3D
 * 
 * @param a the first vector
 * @param b the second vector
 * @cond the placement of the vectors don't matter
 * @return double 
 */
double pyth3(glm::vec3 a, glm::vec3 b);

// macros for calculating the FPS and Delta time
#define CALC_FPS(draw_call, var) std::chrono::time_point<std::chrono::steady_clock> oldTime = std::chrono::high_resolution_clock::now(); draw_call; var = oldTime - std::chrono::high_resolution_clock::now();

/**
 * @brief calculate the deltatime from the current FPS
 * 
 * @return double 
 */
double deltaTime(double FPS);

/**
 * @brief randomly generate a floating point number
 * 
 * @param x bottom limit
 * @param y upper limit
 * @return double 
 */
double randDoub(double x, double y);

} // namespace lyra
