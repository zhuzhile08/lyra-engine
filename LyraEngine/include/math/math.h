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

#include <cmath>
#include <vector>
// #include <math/vectors.h>
// #include <math/matrices.h>
#include <glm.hpp>

namespace lyra {


/** vector type definitions **/

/**
typedef Vector<2, float> Vector2f;
typedef Vector<2, int> Vector2i;
typedef Vector<2, double> Vector2d;

typedef Vector<2, int8> Vector2i8;
typedef Vector<2, int16> Vector2i16;
typedef Vector<2, int32> Vector2i32;
typedef Vector<2, int64> Vector2i64;

typedef Vector<2, uint8> Vector2u8;
typedef Vector<2, uint16> Vector2u16;
typedef Vector<2, uint32> Vector2u32;
typedef Vector<2, uint64> Vector2u64;


typedef Vector<3, float> Vector3f;
typedef Vector<3, int> Vector3i;
typedef Vector<3, double> Vector3d;

typedef Vector<3, int8> Vector3i8;
typedef Vector<3, int16> Vector3i16;
typedef Vector<3, int32> Vector3i33;
typedef Vector<3, int64> Vector3i64;

typedef Vector<3, uint8> Vector3u8;
typedef Vector<3, uint16> Vector3u16;
typedef Vector<3, uint32> Vector3u33;
typedef Vector<3, uint64> Vector3u64;


typedef Vector<4, float> Vector4f;
typedef Vector<4, int> Vector4i;
typedef Vector<4, double> Vector4d;

typedef Vector<4, int8> Vector4i8;
typedef Vector<4, int16> Vector4i16;
typedef Vector<4, int32> Vector4i34;
typedef Vector<4, int64> Vector4i64;

typedef Vector<4, uint8> Vector4u8;
typedef Vector<4, uint16> Vector4u16;
typedef Vector<4, uint32> Vector4u34;
typedef Vector<4, uint64> Vector4u64;
*/

/** matrix type definitions **/

/**
typedef Matrix<2, 2, float> Mat2f;
typedef Matrix<2, 2, int> Mat2i;
typedef Matrix<2, 2, double> Mat2d;

typedef Matrix<2, 2, int8> Mat2i8;
typedef Matrix<2, 2, int16> Mat2i16;
typedef Matrix<2, 2, int32> Mat2i32;
typedef Matrix<2, 2, int64> Mat2i64;

typedef Matrix<2, 2, uint8> Mat2u8;
typedef Matrix<2, 2, uint16> Mat2u16;
typedef Matrix<2, 2, uint32> Mat2u32;
typedef Matrix<2, 2, uint64> Mat2u64;


typedef Matrix<2, 3, float> Mat2x3f;
typedef Matrix<2, 3, int> Mat2x3i;
typedef Matrix<2, 3, double> Mat2x3d;

typedef Matrix<2, 3, int8> Mat2x3i8;
typedef Matrix<2, 3, int16> Mat2x3i16;
typedef Matrix<2, 3, int32> Mat2x3i33;
typedef Matrix<2, 3, int64> Mat2x3i64;

typedef Matrix<2, 3, uint8> Mat2x3u8;
typedef Matrix<2, 3, uint16> Mat2x3u16;
typedef Matrix<2, 3, uint32> Mat2x3u33;
typedef Matrix<2, 3, uint64> Mat2x3u64;


typedef Matrix<2, 4, float> Mat2x4f;
typedef Matrix<2, 4, int> Mat2x4i;
typedef Matrix<2, 4, double> Mat2x4d;

typedef Matrix<2, 4, int8> Mat2x4i8;
typedef Matrix<2, 4, int16> Mat2x4i16;
typedef Matrix<2, 4, int64> Mat2x4i44;
typedef Matrix<2, 4, int64> Mat2x4i64;

typedef Matrix<2, 4, uint8> Mat2x4u8;
typedef Matrix<2, 4, uint16> Mat2x4u16;
typedef Matrix<2, 4, uint64> Mat2x4u44;
typedef Matrix<2, 4, uint64> Mat2x4u64;


typedef Matrix<3, 2, float> Mat3x2f;
typedef Matrix<3, 2, int> Mat3x2i;
typedef Matrix<3, 2, double> Mat3x2d;

typedef Matrix<3, 2, int8> Mat3x2i8;
typedef Matrix<3, 2, int16> Mat3x2i16;
typedef Matrix<3, 2, int32> Mat3x2i32;
typedef Matrix<3, 2, int64> Mat3x2i64;

typedef Matrix<3, 2, uint8> Mat3x2u8;
typedef Matrix<3, 2, uint16> Mat3x2u16;
typedef Matrix<3, 2, uint32> Mat3x2u32;
typedef Matrix<3, 2, uint64> Mat3x2u64;


typedef Matrix<3, 3, float> Mat3f;
typedef Matrix<3, 3, int> Mat3i;
typedef Matrix<3, 3, double> Mat3d;

typedef Matrix<3, 3, int8> Mat3i8;
typedef Matrix<3, 3, int16> Mat3i16;
typedef Matrix<3, 3, int32> Mat3i33;
typedef Matrix<3, 3, int64> Mat3i64;

typedef Matrix<3, 3, uint8> Mat3u8;
typedef Matrix<3, 3, uint16> Mat3u16;
typedef Matrix<3, 3, uint32> Mat3u33;
typedef Matrix<3, 3, uint64> Mat3u64;


typedef Matrix<3, 4, float> Mat3x4f;
typedef Matrix<3, 4, int> Mat3x4i;
typedef Matrix<3, 4, double> Mat3x4d;

typedef Matrix<3, 4, int8> Mat3x4i8;
typedef Matrix<3, 4, int16> Mat3x4i16;
typedef Matrix<3, 4, int64> Mat3x4i44;
typedef Matrix<3, 4, int64> Mat3x4i64;

typedef Matrix<3, 4, uint8> Mat3x4u8;
typedef Matrix<3, 4, uint16> Mat3x4u16;
typedef Matrix<3, 4, uint64> Mat3x4u44;
typedef Matrix<3, 4, uint64> Mat3x4u64;


typedef Matrix<4, 2, float> Mat4x2f;
typedef Matrix<4, 2, int> Mat4x2i;
typedef Matrix<4, 2, double> Mat4x2d;

typedef Matrix<4, 2, int8> Mat4x2i8;
typedef Matrix<4, 2, int16> Mat4x2i16;
typedef Matrix<4, 2, int32> Mat4x2i32;
typedef Matrix<4, 2, int64> Mat4x2i64;

typedef Matrix<4, 2, uint8> Mat4x2u8;
typedef Matrix<4, 2, uint16> Mat4x2u16;
typedef Matrix<4, 2, uint32> Mat4x2u32;
typedef Matrix<4, 2, uint64> Mat4x2u64;


typedef Matrix<4, 3, float> Mat4x3f;
typedef Matrix<4, 3, int> Mat4x3i;
typedef Matrix<4, 3, double> Mat4x3d;

typedef Matrix<4, 3, int8> Mat4x3i8;
typedef Matrix<4, 3, int16> Mat4x3i16;
typedef Matrix<4, 3, int32> Mat4x3i33;
typedef Matrix<4, 3, int64> Mat4x3i64;

typedef Matrix<4, 3, uint8> Mat4x3u8;
typedef Matrix<4, 3, uint16> Mat4x3u16;
typedef Matrix<4, 3, uint32> Mat4x3u33;
typedef Matrix<4, 3, uint64> Mat4x3u64;


typedef Matrix<4, 4, float> glm::mat4;
typedef Matrix<4, 4, int> Mat4i;
typedef Matrix<4, 4, double> Mat4d;

typedef Matrix<4, 4, int8> Mat4i8;
typedef Matrix<4, 4, int16> Mat4i16;
typedef Matrix<4, 4, int64> Mat4i44;
typedef Matrix<4, 4, int64> Mat4i64;

typedef Matrix<4, 4, uint8> Mat4u8;
typedef Matrix<4, 4, uint16> Mat4u16;
typedef Matrix<4, 4, uint64> Mat4u44;
typedef Matrix<4, 4, uint64> Mat4u64;
*/

/**
 * @brief an implementation of the pythagorean theorum
 * 
 * @tparam _Size size of the vectors
 * @tparam _Ty type stored in the vectors
 * @param a first vector
 * @param b second vector
 * 
 * @return _Ty 
 */
template<size_t _Size, class _Ty> NODISCARD _Ty pythagoras(const glm::vec<_Size, _Ty, glm::defaultp>& a, const glm::vec<_Size, _Ty, glm::defaultp>& b);

template<size_t _Size, class _Ty> _Ty pythagoras(const glm::vec<_Size, _Ty, glm::defaultp>& a, const glm::vec<_Size, _Ty, glm::defaultp>& b) {
	_Ty result;
	for (uint8 x = 0; x < _Size; x++) {
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

/**
 * @brief decompose the transformation matrix
 * 
 * @param matrix matrix to decompose
 * @param translation translation vector
 * @param rotation rotation vector
 * @param scale scale vector
 */
void decompose_transform_matrix(const glm::mat4& matrix, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);

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
