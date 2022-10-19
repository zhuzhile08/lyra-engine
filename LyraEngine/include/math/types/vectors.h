/**
 * @file vector3.h
 * @author zhuzhile08 (zhuzhile08@gmail.com)
 * 
 * @brief a simple vector implementation
 * 
 * @date 2022-10-18
 * 
 * @copyright Copyright (c) 2022
 */

namespace lyra {

template <class _Ty> struct Vector2 {
	/**
	 * @brief constuct a new two dimensional vector
	 * 
	 * @param vec vector to construct the value from
	 */
    Vector2(const Vector2<_Ty>& vec) : x(vec.x), y(vec.y) { }/**
	 * @brief constuct a new two dimensional vector
	 * 
	 * @tparam _BTy type of the other vector
	 * @param vec vector to construct the value from
	 */
	template <class _BTy> Vector2(const Vector2<_BTy>& vec) :
		x(static_cast<_Ty>(vec.x)), y(static_cast<_Ty>(vec.y))
	{ }

	/**
	 * @brief constuct a new two dimensional vector
	 * 
	 * @param scalar scalar to initialize all the scalars with
	 */
	Vector2(const _Ty& scalar) : x(scalar), y(scalar) { }
	/**
	 * @brief constuct a new two dimensional vector
	 * 
	 * @tparam _BTy type of the scalar
	 * @param scalar scalar to initialize all the scalars with
	 */
	template <class _BTy> Vector2(const _BTy& scalar) :
		x(static_cast<_Ty>(scalar)), y(static_cast<_Ty>(scalar))
	{ }

	/**
	 * @brief constuct a new two dimensional vector
	 * 
	 * @param x x value
	 * @param y y value
	 */
	Vector2(const _Ty& x, const _Ty& y) : x(x), y(y) { }	
	/**
	 * @brief constuct a new two dimensional vector
	 * 
	 * @tparam X type of the x value
	 * @tparam Y type of the y value
	 * @param _x x value
	 * @param _y y value
	 */
	template <class X, class Y> Vector2(const X& _x, const Y& _y) :
		x(static_cast<_Ty>(_x)), y(static_cast<_Ty>(y))
	{ }

	/** unary arthmetric operators **/

	NODISCARD CONSTEXPR Vector2<_Ty>& operator=(const Vector2<_Ty>& vec) {
		x = vec.x;
		y = vec.y;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector2<_Ty>& operator=(const Vector2<_BTy>& vec) {
		*this = Vector2<_Ty>(vec);
		return *this;
	}
	NODISCARD CONSTEXPR Vector2<_Ty>& operator=(const _Ty& scalar) {
		x = scalar;
		y = scalar;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector2<_Ty>& operator=(const _BTy& scalar) {
		x = static_cast<_Ty>(scalar);
		y = static_cast<_Ty>(scalar);
		return *this;
	}

	NODISCARD CONSTEXPR Vector2<_Ty>& operator+=(const Vector2<_Ty>& vec) {
		x += vec.x;
		y += vec.y;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector2<_Ty>& operator+=(const Vector2<_BTy>& vec) {
		*this += Vector2<_Ty>(vec);
		return *this;
	}
	NODISCARD CONSTEXPR Vector2<_Ty>& operator+=(const _Ty& scalar) {
		x += scalar;
		y += scalar;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector2<_Ty>& operator+=(const _BTy& scalar) {
		x += static_cast<_Ty>(scalar);
		y += static_cast<_Ty>(scalar);
		return *this;
	}

	NODISCARD CONSTEXPR Vector2<_Ty>& operator-=(const Vector2<_Ty>& vec) {
		x -= vec.x;
		y -= vec.y;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector2<_Ty>& operator-=(const Vector2<_BTy>& vec) {
		*this -= Vector2<_Ty>(vec);
		return *this;
	}
	NODISCARD CONSTEXPR Vector2<_Ty>& operator-=(const _Ty& scalar) {
		x -= scalar;
		y -= scalar;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector2<_Ty>& operator-=(const _BTy& scalar) {
		x -= static_cast<_Ty>(scalar);
		y -= static_cast<_Ty>(scalar);
		return *this;
	}

	NODISCARD CONSTEXPR Vector2<_Ty>& operator*=(const Vector2<_Ty>& vec) {
		x *= vec.x;
		y *= vec.y;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector2<_Ty>& operator*=(const Vector2<_BTy>& vec) {
		*this *= Vector2<_Ty>(vec);
		return *this;
	}
	NODISCARD CONSTEXPR Vector2<_Ty>& operator*=(const _Ty& scalar) {
		x *= scalar;
		y *= scalar;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector2<_Ty>& operator*=(const _BTy& scalar) {
		x *= static_cast<_Ty>(scalar);
		y *= static_cast<_Ty>(scalar);
		return *this;
	}

	NODISCARD CONSTEXPR Vector2<_Ty>& operator/=(const Vector2<_Ty>& vec) {
		x /= vec.x;
		y /= vec.y;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector2<_Ty>& operator/=(const Vector2<_BTy>& vec) {
		*this /= Vector2<_Ty>(vec);
		return *this;
	}
	NODISCARD CONSTEXPR Vector2<_Ty>& operator/=(const _Ty& scalar) {
		x /= scalar;
		y /= scalar;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector2<_Ty>& operator/=(const _BTy& scalar) {
		x /= static_cast<_Ty>(scalar);
		y /= static_cast<_Ty>(scalar);
		return *this;
	}
	
	NODISCARD CONSTEXPR Vector2<_Ty>& operator%=(const Vector2<_Ty>& vec) {
		x %= vec.x;
		y %= vec.y;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector2<_Ty>& operator%=(const Vector2<_BTy>& vec) {
		*this %= Vector2<_Ty>(vec);
		return *this;
	}
	NODISCARD CONSTEXPR Vector2<_Ty>& operator%=(const _Ty& scalar) {
		x %= scalar;
		y %= scalar;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector2<_Ty>& operator%=(const _BTy& scalar) {
		x %= static_cast<_Ty>(scalar);
		y %= static_cast<_Ty>(scalar);
		return *this;
	}

	NODISCARD CONSTEXPR Vector2<_Ty>& operator++() {
		x++;
		y++;
		return *this;
	}
	NODISCARD CONSTEXPR Vector2<_Ty>& operator--() {
		x--;
		y--;
		return *this;
	}
	NODISCARD CONSTEXPR Vector2<_Ty>& operator++(int) {
		Vector2<_Ty> original(*this);
		++*this;
		return original;
	}
	NODISCARD CONSTEXPR Vector2<_Ty>& operator--(int) {
		Vector2<_Ty> original(*this);
		++*this;
		return original;
	}


	/** boolean operators **/

	NODISCARD CONSTEXPR bool operator==(const Vector2<_Ty>& vec) {
		return x == vec.x && y == vec.y;
	}
	NODISCARD CONSTEXPR bool operator!=(const Vector2<_Ty>& vec) {
		return !(*this == vec);
	}


	/** member access operators **/

	NODISCARD CONSTEXPR _Ty& operator[] (size_t index) {
		switch (index) {
			case 0:
				return x;
			case 1:
				return y;
			default:
#ifndef NDEBUG
				Logger::log_exception("An invalid component access index of: ", index, " was passed to [] operator in the Vector2 at: ", get_address(this), "!");
				return _Ty();
#else
				return _Ty();
#endif
		}
	}
	NODISCARD CONSTEXPR const _Ty& operator[] (size_t index) const {
		switch (index) {
			case 0:
				return x;
			case 1:
				return y;
			default:
#ifndef NDEBUG
				Logger::log_exception("An invalid component access index of: ", index, " was passed to [] operator in the Vector2 at: ", get_address(this), "!");
				return _Ty();
#else
				return _Ty();
#endif
		}
	}
    
    _Ty x, y;
	// this will also be one of the times where I will deviate from my normal naming conventions for readability's sake
};

template <class _Ty> struct Vector3 {
	/**
	 * @brief constuct a new three dimensional vector
	 * 
	 * @param vec vector to construct the value from
	 */
    Vector3(const Vector3<_Ty>& vec) : x(vec.x), y(vec.y), z(vec.z) { }
	/**
	 * @brief constuct a new three dimensional vector
	 * 
	 * @tparam _BTy type of the other vector
	 * @param vec vector to construct the value from
	 */
	template <class _BTy> Vector3(const Vector3<_BTy>& vec) :
		x(static_cast<_Ty>(vec.x)), y(static_cast<_Ty>(vec.y)), z(static_cast<_Ty>(vec.z))
	{ }
	
	/**
	 * @brief constuct a new three dimensional vector
	 * 
	 * @param scalar scalar to initialize all the scalars with
	 */
	Vector3(const _Ty& scalar) : x(scalar), y(scalar), z(scalar) { }
	/**
	 * @brief constuct a new three dimensional vector
	 * 
	 * @tparam _BTy type of the scalar
	 * @param scalar scalar to initialize all the scalars with
	 */
	template <class _BTy> Vector3(const _BTy& scalar) :
		x(static_cast<_Ty>(scalar)), y(static_cast<_Ty>(scalar)), z(static_cast<_Ty>(scalar)) 
	{ }

	/**
	 * @brief constuct a new three dimensional vector
	 * 
	 * @param x x value
	 * @param y y value
	 * @param z z value
	 */
	Vector3(const _Ty& x, const _Ty& y, const _Ty& z) : x(x), y(y), z(z) { }	
	/**
	 * @brief constuct a new three dimensional vector
	 * 
	 * @tparam X type of the x value
	 * @tparam Y type of the y value
	 * @tparam Z type of the z value
	 * @param _x x value
	 * @param _y y value
	 * @param _z z value
	 */
	template <class X, class Y, class Z> Vector3(const X& _x, const Y& _y, const Z& _z) :
		x(static_cast<_Ty>(_x)), y(static_cast<_Ty>(y)), z(static_cast<_Ty>(z)) 
	{ }

	/** unary arthmetric operators **/

	NODISCARD CONSTEXPR Vector3<_Ty>& operator=(const Vector3<_Ty>& vec) {
		x = vec.x;
		y = vec.y;
		z = vec.z;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector3<_Ty>& operator=(const Vector3<_BTy>& vec) {
		*this = Vector3<_Ty>(vec);
		return *this;
	}
	NODISCARD CONSTEXPR Vector3<_Ty>& operator=(const _Ty& scalar) {
		x = scalar;
		y = scalar;
		z = scalar;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector3<_Ty>& operator=(const _BTy& scalar) {
		x = static_cast<_Ty>(scalar);
		y = static_cast<_Ty>(scalar);
		z = static_cast<_Ty>(scalar);
		return *this;
	}

	NODISCARD CONSTEXPR Vector3<_Ty>& operator+=(const Vector3<_Ty>& vec) {
		x += vec.x;
		y += vec.y;
		z += vec.z;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector3<_Ty>& operator+=(const Vector3<_BTy>& vec) {
		*this += Vector3<_Ty>(vec);
		return *this;
	}
	NODISCARD CONSTEXPR Vector3<_Ty>& operator+=(const _Ty& scalar) {
		x += scalar;
		y += scalar;
		z += scalar;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector3<_Ty>& operator+=(const _BTy& scalar) {
		x += static_cast<_Ty>(scalar);
		y += static_cast<_Ty>(scalar);
		z += static_cast<_Ty>(scalar);
		return *this;
	}

	NODISCARD CONSTEXPR Vector3<_Ty>& operator-=(const Vector3<_Ty>& vec) {
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector3<_Ty>& operator-=(const Vector3<_BTy>& vec) {
		*this -= Vector3<_Ty>(vec);
		return *this;
	}
	NODISCARD CONSTEXPR Vector3<_Ty>& operator-=(const _Ty& scalar) {
		x -= scalar;
		y -= scalar;
		z -= scalar;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector3<_Ty>& operator-=(const _BTy& scalar) {
		x -= static_cast<_Ty>(scalar);
		y -= static_cast<_Ty>(scalar);
		z -= static_cast<_Ty>(scalar);
		return *this;
	}

	NODISCARD CONSTEXPR Vector3<_Ty>& operator*=(const Vector3<_Ty>& vec) {
		x *= vec.x;
		y *= vec.y;
		z *= vec.z;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector3<_Ty>& operator*=(const Vector3<_BTy>& vec) {
		*this *= Vector3<_Ty>(vec);
		return *this;
	}
	NODISCARD CONSTEXPR Vector3<_Ty>& operator*=(const _Ty& scalar) {
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector3<_Ty>& operator*=(const _BTy& scalar) {
		x *= static_cast<_Ty>(scalar);
		y *= static_cast<_Ty>(scalar);
		z *= static_cast<_Ty>(scalar);
		return *this;
	}

	NODISCARD CONSTEXPR Vector3<_Ty>& operator/=(const Vector3<_Ty>& vec) {
		x /= vec.x;
		y /= vec.y;
		z /= vec.z;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector3<_Ty>& operator/=(const Vector3<_BTy>& vec) {
		*this /= Vector3<_Ty>(vec);
		return *this;
	}
	NODISCARD CONSTEXPR Vector3<_Ty>& operator/=(const _Ty& scalar) {
		x /= scalar;
		y /= scalar;
		z /= scalar;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector3<_Ty>& operator/=(const _BTy& scalar) {
		x /= static_cast<_Ty>(scalar);
		y /= static_cast<_Ty>(scalar);
		z /= static_cast<_Ty>(scalar);
		return *this;
	}

	NODISCARD CONSTEXPR Vector3<_Ty>& operator%=(const Vector3<_Ty>& vec) {
		x %= vec.x;
		y %= vec.y;
		z %= vec.z;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector3<_Ty>& operator%=(const Vector3<_BTy>& vec) {
		*this %= Vector3<_Ty>(vec);
		return *this;
	}
	NODISCARD CONSTEXPR Vector3<_Ty>& operator%=(const _Ty& scalar) {
		x %= scalar;
		y %= scalar;
		z %= scalar;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector3<_Ty>& operator%=(const _BTy& scalar) {
		x %= static_cast<_Ty>(scalar);
		y %= static_cast<_Ty>(scalar);
		z %= static_cast<_Ty>(scalar);
		return *this;
	}

	NODISCARD CONSTEXPR Vector3<_Ty>& operator++() {
		x++;
		y++;
		z++;
		return *this;
	}
	NODISCARD CONSTEXPR Vector3<_Ty>& operator--() {
		x--;
		y--;
		z--;
		return *this;
	}
	NODISCARD CONSTEXPR Vector3<_Ty>& operator++(int) {
		Vector3<_Ty> original(*this);
		++*this;
		return original;
	}
	NODISCARD CONSTEXPR Vector3<_Ty>& operator--(int) {
		Vector3<_Ty> original(*this);
		++*this;
		return original;
	}


	/** boolean operators **/

	NODISCARD CONSTEXPR bool operator==(const Vector3<_Ty>& vec) {
		return x == vec.x && y == vec.y && z == vec.z;
	}
	NODISCARD CONSTEXPR bool operator!=(const Vector3<_Ty>& vec) {
		return !(*this == vec);
	}


	/** member access operators **/

	NODISCARD CONSTEXPR _Ty& operator[] (const size_t& index) {
		switch (index) {
			case 0:
				return x;
			case 1:
				return y;
			case 2:
				return z;
			default:
#ifndef NDEBUG
				Logger::log_exception("An invalid component access index of: ", index, " was passed to [] operator in the Vector3 at: ", get_address(this), "!");
				return _Ty();
#else
				return _Ty();
#endif
		}
	}
	NODISCARD CONSTEXPR const _Ty& operator[] (const size_t& index) const {
		switch (index) {
			case 0:
				return x;
			case 1:
				return y;
			case 2:
				return z;
			default:
#ifndef NDEBUG
				Logger::log_exception("An invalid component access index of: ", index, " was passed to [] operator in the Vector3 at: ", get_address(this), "!");
				return _Ty();
#else
				return _Ty();
#endif
		}
	}

    _Ty x, y, z;
	// this will also be one of the times where I will deviate from my normal naming conventions for readability's sake
};

template <class _Ty> struct Vector4 {
	/**
	 * @brief constuct a new three dimensional vector
	 * 
	 * @param vec vector to construct the value from
	 */
    Vector4(const Vector4<_Ty>& vec) : x(vec.x), y(vec.y), z(vec.z), w(vec.w) { }
	/**
	 * @brief constuct a new three dimensional vector
	 * 
	 * @tparam _BTy type of the other vector
	 * @param vec vector to construct the value from
	 */
	template <class _BTy> Vector4(const Vector4<_BTy>& vec) :
		x(static_cast<_Ty>(vec.x)), y(static_cast<_Ty>(vec.y), z(static_cast<_Ty>(vec.z)), w(static_cast<_Ty>(vec.w))) 
	{ }
	
	/**
	 * @brief constuct a new three dimensional vector
	 * 
	 * @param scalar scalar to initialize all the scalars with
	 */
	Vector4(const _Ty& scalar) : x(scalar), y(scalar), z(scalar), w(scalar) { }
	/**
	 * @brief constuct a new three dimensional vector
	 * 
	 * @tparam _BTy type of the scalar
	 * @param scalar scalar to initialize all the scalars with
	 */
	template <class _BTy> Vector4(const _BTy& scalar) :
		x(static_cast<_Ty>(scalar)), 
		y(static_cast<_Ty>(scalar)), 
		z(static_cast<_Ty>(scalar)), 
		w(static_cast<_Ty>(scalar))
	{ }

	/**
	 * @brief constuct a new three dimensional vector
	 * 
	 * @param x x value
	 * @param y y value
	 * @param z z value
	 */
	Vector4(const _Ty& x, const _Ty& y, const _Ty& z, const _Ty& w) : x(x), y(y), z(z), w(w) { }	
	/**
	 * @brief constuct a new three dimensional vector
	 * 
	 * @tparam X type of the x value
	 * @tparam Y type of the y value
	 * @tparam Z type of the z value
	 * @tparam W type of the w value
	 * @param _x x value
	 * @param _y y value
	 * @param _z z value
	 * @param _w w value
	 */
	template <class X, class Y, class Z, class W> Vector4(const X& _x, const Y& _y, const Z& _z ,const W& _w) :
		x(static_cast<_Ty>(_x)), y(static_cast<_Ty>(_y)), z(static_cast<_Ty>(_z)), w(static_cast<_Ty>(_w))
	{ }

	/** unary operators **/

	NODISCARD CONSTEXPR Vector4<_Ty>& operator=(const Vector4<_Ty>& vec) {
		x = vec.x;
		y = vec.y;
		z = vec.z;
		w = vec.z;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector4<_Ty>& operator=(const Vector4<_BTy>& vec) {
		*this = Vector4<_Ty>(vec);
		return *this;
	}
	NODISCARD CONSTEXPR Vector4<_Ty>& operator=(const _Ty& scalar) {
		x = scalar;
		y = scalar;
		z = scalar;
		w = scalar;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector4<_Ty>& operator=(const _BTy& scalar) {
		x = static_cast<_Ty>(scalar);
		y = static_cast<_Ty>(scalar);
		z = static_cast<_Ty>(scalar);
		w = static_cast<_Ty>(scalar);
		return *this;
	}

	NODISCARD CONSTEXPR Vector4<_Ty>& operator+=(const Vector4<_Ty>& vec) {
		x += vec.x;
		y += vec.y;
		z += vec.z;
		w += vec.z;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector4<_Ty>& operator+=(const Vector4<_BTy>& vec) {
		*this += Vector4<_Ty>(vec);
		return *this;
	}
	NODISCARD CONSTEXPR Vector4<_Ty>& operator+=(const _Ty& scalar) {
		x += scalar;
		y += scalar;
		z += scalar;
		w += scalar;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector4<_Ty>& operator+=(const _BTy& scalar) {
		x += static_cast<_Ty>(scalar);
		y += static_cast<_Ty>(scalar);
		z += static_cast<_Ty>(scalar);
		w += static_cast<_Ty>(scalar);
		return *this;
	}

	NODISCARD CONSTEXPR Vector4<_Ty>& operator-=(const Vector4<_Ty>& vec) {
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;
		w -= vec.z;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector4<_Ty>& operator-=(const Vector4<_BTy>& vec) {
		*this -= Vector4<_Ty>(vec);
		return *this;
	}
	NODISCARD CONSTEXPR Vector4<_Ty>& operator-=(const _Ty& scalar) {
		x -= scalar;
		y -= scalar;
		z -= scalar;
		w -= scalar;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector4<_Ty>& operator-=(const _BTy& scalar) {
		x -= static_cast<_Ty>(scalar);
		y -= static_cast<_Ty>(scalar);
		z -= static_cast<_Ty>(scalar);
		w -= static_cast<_Ty>(scalar);
		return *this;
	}

	NODISCARD CONSTEXPR Vector4<_Ty>& operator*=(const Vector4<_Ty>& vec) {
		x *= vec.x;
		y *= vec.y;
		z *= vec.z;
		w *= vec.w;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector4<_Ty>& operator*=(const Vector4<_BTy>& vec) {
		*this *= Vector4<_Ty>(vec);
		return *this;
	}
	NODISCARD CONSTEXPR Vector4<_Ty>& operator*=(const _Ty& scalar) {
		x *= scalar;
		y *= scalar;
		z *= scalar;
		w *= scalar;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector4<_Ty>& operator*=(const _BTy& scalar) {
		x *= static_cast<_Ty>(scalar);
		y *= static_cast<_Ty>(scalar);
		z *= static_cast<_Ty>(scalar);
		w *= static_cast<_Ty>(scalar);
		return *this;
	}

	NODISCARD CONSTEXPR Vector4<_Ty>& operator/=(const Vector4<_Ty>& vec) {
		x /= vec.x;
		y /= vec.y;
		z /= vec.z;
		w /= vec.w;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector4<_Ty>& operator/=(const Vector4<_BTy>& vec) {
		*this /= Vector4<_Ty>(vec);
		return *this;
	}
	NODISCARD CONSTEXPR Vector4<_Ty>& operator/=(const _Ty& scalar) {
		x /= scalar;
		y /= scalar;
		z /= scalar;
		w /= scalar;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector4<_Ty>& operator/=(const _BTy& scalar) {
		x /= static_cast<_Ty>(scalar);
		y /= static_cast<_Ty>(scalar);
		z /= static_cast<_Ty>(scalar);
		w /= static_cast<_Ty>(scalar);
		return *this;
	}

	NODISCARD CONSTEXPR Vector4<_Ty>& operator%=(const Vector4<_Ty>& vec) {
		x %= vec.x;
		y %= vec.y;
		z %= vec.z;
		w %= vec.w;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector4<_Ty>& operator%=(const Vector4<_BTy>& vec) {
		*this %= Vector4<_Ty>(vec);
		return *this;
	}
	NODISCARD CONSTEXPR Vector4<_Ty>& operator%=(const _Ty& scalar) {
		x %= scalar;
		y %= scalar;
		z %= scalar;
		w %= scalar;
		return *this;
	}
	template<class _BTy> NODISCARD CONSTEXPR Vector4<_Ty>& operator%=(const _BTy& scalar) {
		x %= static_cast<_Ty>(scalar);
		y %= static_cast<_Ty>(scalar);
		z %= static_cast<_Ty>(scalar);
		w %= static_cast<_Ty>(scalar);
		return *this;
	}

	NODISCARD CONSTEXPR Vector4<_Ty>& operator++() {
		x++;
		y++;
		z++;
		w++;
		return *this;
	}
	NODISCARD CONSTEXPR Vector4<_Ty>& operator--() {
		x--;
		y--;
		z--;
		w--;
		return *this;
	}
	NODISCARD CONSTEXPR Vector4<_Ty>& operator++(int) {
		Vector4<_Ty> original(*this);
		++*this;
		return original;
	}
	NODISCARD CONSTEXPR Vector4<_Ty>& operator--(int) {
		Vector4<_Ty> original(*this);
		++*this;
		return original;
	}


	/** boolean operators **/

	NODISCARD CONSTEXPR bool operator==(const Vector4<_Ty>& vec) {
		return x == vec.x && y == vec.y && z == vec.z && w = vec.w;
	}
	NODISCARD CONSTEXPR bool operator!=(const Vector4<_Ty>& vec) {
		return !(*this == vec);
	}


	/** member access operators **/

	NODISCARD CONSTEXPR _Ty& operator[] (const size_t& index) {
		switch (index) {
			case 0:
				return x;
			case 1:
				return y;
			case 2:
				return z;
			case 4:
				return w;
			default:
#ifndef NDEBUG
				Logger::log_exception("An invalid component access index of: ", index, " was passed to [] operator in the Vector4 at: ", get_address(this), "!");
				return _Ty();
#else
				return _Ty();
#endif
		}
	}
	NODISCARD CONSTEXPR const _Ty& operator[] (const size_t& index) const {
		switch (index) {
			case 0:
				return x;
			case 1:
				return y;
			case 2:
				return z;
			case 4:
				return w;
			default:
#ifndef NDEBUG
				Logger::log_exception("An invalid component access index of: ", index, " was passed to [] operator in the Vector4 at: ", get_address(this), "!");
				return _Ty();
#else
				return _Ty();
#endif
		}
	}
    
    _Ty x, y, z, w;
	// this will also be one of the times where I will deviate from my normal naming conventions for readability's sake
};

// color
typedef Vector4<uint8> Color;
// bri'ish colour
typedef Color Colour;

/** Vector 2 **/

template<class _Ty> NODISCARD CONSTEXPR Vector2<_Ty>& operator+(const Vector2<_Ty>& vec, const Vector2<_Ty>& vec2) {
	return Vector2<_Ty>(
		vec.x + vec2.x,
		vec.y + vec2.y
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector2<_Ty>& operator+(const Vector2<_Ty>& vec, const Vector2<_BTy>& vec2) {
	return Vector2<_Ty>(vec + Vector2<_Ty>(vec2));
}
template<class _Ty> NODISCARD CONSTEXPR Vector2<_Ty>& operator+(const Vector2<_Ty>& vec, const _Ty& scalar) {
	return Vector2<_Ty>(
		vec.x + scalar,
		vec.y + scalar
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector2<_Ty>& operator+(const Vector2<_Ty>& vec, const _BTy& scalar) {
	return Vector2<_Ty>(
		vec.x + static_cast<_Ty>(scalar),
		vec.y + static_cast<_Ty>(scalar)
	);
}
template<class _Ty> NODISCARD CONSTEXPR Vector2<_Ty>& operator+(const _Ty& scalar, const Vector2<_Ty>& vec) {
	return Vector2<_Ty>(
		vec.x + scalar,
		vec.y + scalar
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector2<_Ty>& operator+(const _BTy& scalar, const Vector2<_Ty>& vec) {
	return Vector2<_Ty>(
		vec.x + static_cast<_Ty>(scalar),
		vec.y + static_cast<_Ty>(scalar)
	);
}

template<class _Ty> NODISCARD CONSTEXPR Vector2<_Ty>& operator-(const Vector2<_Ty>& vec, const Vector2<_Ty>& vec2) {
	return Vector2<_Ty>(
		vec.x - vec2.x,
		vec.y - vec2.y
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector2<_Ty>& operator-(const Vector2<_Ty>& vec, const Vector2<_BTy>& vec2) {
	return Vector2<_Ty>(vec - Vector2<_Ty>(vec2));
}
template<class _Ty> NODISCARD CONSTEXPR Vector2<_Ty>& operator-(const Vector2<_Ty>& vec, const _Ty& scalar) {
	return Vector2<_Ty>(
		vec.x - scalar,
		vec.y - scalar
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector2<_Ty>& operator-(const Vector2<_Ty>& vec, const _BTy& scalar) {
	return Vector2<_Ty>(
		vec.x - static_cast<_Ty>(scalar),
		vec.y - static_cast<_Ty>(scalar)
	);
}
template<class _Ty> NODISCARD CONSTEXPR Vector2<_Ty>& operator-(const _Ty& scalar, const Vector2<_Ty>& vec) {
	return Vector2<_Ty>(
		vec.x - scalar,
		vec.y - scalar
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector2<_Ty>& operator-(const _BTy& scalar, const Vector2<_Ty>& vec) {
	return Vector2<_Ty>(
		vec.x - static_cast<_Ty>(scalar),
		vec.y - static_cast<_Ty>(scalar)
	);
}

template<class _Ty> NODISCARD CONSTEXPR Vector2<_Ty>& operator*(const Vector2<_Ty>& vec, const Vector2<_Ty>& vec2) {
	return Vector2<_Ty>(
		vec.x * vec2.x,
		vec.y * vec2.y
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector2<_Ty>& operator*(const Vector2<_Ty>& vec, const Vector2<_BTy>& vec2) {
	return Vector2<_Ty>(vec * Vector2<_Ty>(vec2));
}
template<class _Ty> NODISCARD CONSTEXPR Vector2<_Ty>& operator*(const Vector2<_Ty>& vec, const _Ty& scalar) {
	return Vector2<_Ty>(
		vec.x * scalar,
		vec.y * scalar
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector2<_Ty>& operator*(const Vector2<_Ty>& vec, const _BTy& scalar) {
	return Vector2<_Ty>(
		vec.x * static_cast<_Ty>(scalar),
		vec.y * static_cast<_Ty>(scalar)
	);
}
template<class _Ty> NODISCARD CONSTEXPR Vector2<_Ty>& operator*(const _Ty& scalar, const Vector2<_Ty>& vec) {
	return Vector2<_Ty>(
		vec.x * scalar,
		vec.y * scalar
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector2<_Ty>& operator*(const _BTy& scalar, const Vector2<_Ty>& vec) {
	return Vector2<_Ty>(
		vec.x * static_cast<_Ty>(scalar),
		vec.y * static_cast<_Ty>(scalar)
	);
}

template<class _Ty> NODISCARD CONSTEXPR Vector2<_Ty>& operator/(const Vector2<_Ty>& vec, const Vector2<_Ty>& vec2) {
	return Vector2<_Ty>(
		vec.x / vec2.x,
		vec.y / vec2.y
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector2<_Ty>& operator/(const Vector2<_Ty>& vec, const Vector2<_BTy>& vec2) {
	return Vector2<_Ty>(vec / Vector2<_Ty>(vec2));
}
template<class _Ty> NODISCARD CONSTEXPR Vector2<_Ty>& operator/(const Vector2<_Ty>& vec, const _Ty& scalar) {
	return Vector2<_Ty>(
		vec.x / scalar,
		vec.y / scalar
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector2<_Ty>& operator/(const Vector2<_Ty>& vec, const _BTy& scalar) {
	return Vector2<_Ty>(
		vec.x / static_cast<_Ty>(scalar),
		vec.y / static_cast<_Ty>(scalar)
	);
}
template<class _Ty> NODISCARD CONSTEXPR Vector2<_Ty>& operator/(const _Ty& scalar, const Vector2<_Ty>& vec) {
	return Vector2<_Ty>(
		vec.x / scalar,
		vec.y / scalar
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector2<_Ty>& operator/(const _BTy& scalar, const Vector2<_Ty>& vec) {
	return Vector2<_Ty>(
		vec.x / static_cast<_Ty>(scalar),
		vec.y / static_cast<_Ty>(scalar)
	);
}

template<class _Ty> NODISCARD CONSTEXPR Vector2<_Ty>& operator%(const Vector2<_Ty>& vec, const Vector2<_Ty>& vec2) {
	return Vector2<_Ty>(
		vec.x % vec2.x,
		vec.y % vec2.y
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector2<_Ty>& operator%(const Vector2<_Ty>& vec, const Vector2<_BTy>& vec2) {
	return Vector2<_Ty>(vec % Vector2<_Ty>(vec2));
}
template<class _Ty> NODISCARD CONSTEXPR Vector2<_Ty>& operator%(const Vector2<_Ty>& vec, const _Ty& scalar) {
	return Vector2<_Ty>(
		vec.x % scalar,
		vec.y % scalar
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector2<_Ty>& operator%(const Vector2<_Ty>& vec, const _BTy& scalar) {
	return Vector2<_Ty>(
		vec.x % static_cast<_Ty>(scalar),
		vec.y % static_cast<_Ty>(scalar)
	);
}
template<class _Ty> NODISCARD CONSTEXPR Vector2<_Ty>& operator%(const _Ty& scalar, const Vector2<_Ty>& vec) {
	return Vector2<_Ty>(
		vec.x % scalar,
		vec.y % scalar
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector2<_Ty>& operator%(const _BTy& scalar, const Vector2<_Ty>& vec) {
	return Vector2<_Ty>(
		vec.x % static_cast<_Ty>(scalar),
		vec.y % static_cast<_Ty>(scalar)
	);
}

/** Vector 3 **/

template<class _Ty> NODISCARD CONSTEXPR Vector3<_Ty>& operator+(const Vector3<_Ty>& vec, const Vector3<_Ty>& vec2) {
	return Vector3<_Ty>(
		vec.x + vec2.x,
		vec.y + vec2.y,
		vec.z + vec2.z
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector3<_Ty>& operator+(const Vector3<_Ty>& vec, const Vector3<_BTy>& vec2) {
	return Vector3<_Ty>(vec + Vector3<_Ty>(vec2));
}
template<class _Ty> NODISCARD CONSTEXPR Vector3<_Ty>& operator+(const Vector3<_Ty>& vec, const _Ty& scalar) {
	return Vector3<_Ty>(
		vec.x + scalar,
		vec.y + scalar,
		vec.z + scalar
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector3<_Ty>& operator+(const Vector3<_Ty>& vec, const _BTy& scalar) {
	return Vector3<_Ty>(
		vec.x + static_cast<_Ty>(scalar),
		vec.y + static_cast<_Ty>(scalar),
		vec.z + static_cast<_Ty>(scalar)
	);
}
template<class _Ty> NODISCARD CONSTEXPR Vector3<_Ty>& operator+(const _Ty& scalar, const Vector3<_Ty>& vec) {
	return Vector3<_Ty>(
		vec.x + scalar,
		vec.y + scalar,
		vec.z + scalar
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector3<_Ty>& operator+(const _BTy& scalar, const Vector3<_Ty>& vec) {
	return Vector3<_Ty>(
		vec.x + static_cast<_Ty>(scalar),
		vec.y + static_cast<_Ty>(scalar),
		vec.z + static_cast<_Ty>(scalar)
	);
}

template<class _Ty> NODISCARD CONSTEXPR Vector3<_Ty>& operator-(const Vector3<_Ty>& vec, const Vector3<_Ty>& vec2) {
	return Vector3<_Ty>(
		vec.x - vec2.x,
		vec.y - vec2.y,
		vec.z - vec2.z
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector3<_Ty>& operator-(const Vector3<_Ty>& vec, const Vector3<_BTy>& vec2) {
	return Vector3<_Ty>(vec - Vector3<_Ty>(vec2));
}
template<class _Ty> NODISCARD CONSTEXPR Vector3<_Ty>& operator-(const Vector3<_Ty>& vec, const _Ty& scalar) {
	return Vector3<_Ty>(
		vec.x - scalar,
		vec.y - scalar,
		vec.z - scalar
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector3<_Ty>& operator-(const Vector3<_Ty>& vec, const _BTy& scalar) {
	return Vector3<_Ty>(
		vec.x - static_cast<_Ty>(scalar),
		vec.y - static_cast<_Ty>(scalar),
		vec.z - static_cast<_Ty>(scalar)
	);
}
template<class _Ty> NODISCARD CONSTEXPR Vector3<_Ty>& operator-(const _Ty& scalar, const Vector3<_Ty>& vec) {
	return Vector3<_Ty>(
		vec.x - scalar,
		vec.y - scalar,
		vec.z - scalar
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector3<_Ty>& operator-(const _BTy& scalar, const Vector3<_Ty>& vec) {
	return Vector3<_Ty>(
		vec.x - static_cast<_Ty>(scalar),
		vec.y - static_cast<_Ty>(scalar),
		vec.z - static_cast<_Ty>(scalar)
	);
}

template<class _Ty> NODISCARD CONSTEXPR Vector3<_Ty>& operator*(const Vector3<_Ty>& vec, const Vector3<_Ty>& vec2) {
	return Vector3<_Ty>(
		vec.x * vec2.x,
		vec.y * vec2.y,
		vec.z * vec2.z
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector3<_Ty>& operator*(const Vector3<_Ty>& vec, const Vector3<_BTy>& vec2) {
	return Vector3<_Ty>(vec * Vector3<_Ty>(vec2));
}
template<class _Ty> NODISCARD CONSTEXPR Vector3<_Ty>& operator*(const Vector3<_Ty>& vec, const _Ty& scalar) {
	return Vector3<_Ty>(
		vec.x * scalar,
		vec.y * scalar,
		vec.z * scalar
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector3<_Ty>& operator*(const Vector3<_Ty>& vec, const _BTy& scalar) {
	return Vector3<_Ty>(
		vec.x * static_cast<_Ty>(scalar),
		vec.y * static_cast<_Ty>(scalar),
		vec.z * static_cast<_Ty>(scalar)
	);
}
template<class _Ty> NODISCARD CONSTEXPR Vector3<_Ty>& operator*(const _Ty& scalar, const Vector3<_Ty>& vec) {
	return Vector3<_Ty>(
		vec.x * scalar,
		vec.y * scalar,
		vec.z * scalar
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector3<_Ty>& operator*(const _BTy& scalar, const Vector3<_Ty>& vec) {
	return Vector3<_Ty>(
		vec.x * static_cast<_Ty>(scalar),
		vec.y * static_cast<_Ty>(scalar),
		vec.z * static_cast<_Ty>(scalar)
	);
}

template<class _Ty> NODISCARD CONSTEXPR Vector3<_Ty>& operator/(const Vector3<_Ty>& vec, const Vector3<_Ty>& vec2) {
	return Vector3<_Ty>(
		vec.x / vec2.x,
		vec.y / vec2.y,
		vec.z / vec2.z
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector3<_Ty>& operator/(const Vector3<_Ty>& vec, const Vector3<_BTy>& vec2) {
	return Vector3<_Ty>(vec / Vector3<_Ty>(vec2));
}
template<class _Ty> NODISCARD CONSTEXPR Vector3<_Ty>& operator/(const Vector3<_Ty>& vec, const _Ty& scalar) {
	return Vector3<_Ty>(
		vec.x / scalar,
		vec.y / scalar,
		vec.z / scalar
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector3<_Ty>& operator/(const Vector3<_Ty>& vec, const _BTy& scalar) {
	return Vector3<_Ty>(
		vec.x / static_cast<_Ty>(scalar),
		vec.y / static_cast<_Ty>(scalar),
		vec.z / static_cast<_Ty>(scalar)
	);
}
template<class _Ty> NODISCARD CONSTEXPR Vector3<_Ty>& operator/(const _Ty& scalar, const Vector3<_Ty>& vec) {
	return Vector3<_Ty>(
		vec.x / scalar,
		vec.y / scalar,
		vec.z / scalar
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector3<_Ty>& operator/(const _BTy& scalar, const Vector3<_Ty>& vec) {
	return Vector3<_Ty>(
		vec.x / static_cast<_Ty>(scalar),
		vec.y / static_cast<_Ty>(scalar),
		vec.z / static_cast<_Ty>(scalar)
	);
}

template<class _Ty> NODISCARD CONSTEXPR Vector3<_Ty>& operator%(const Vector3<_Ty>& vec, const Vector3<_Ty>& vec2) {
	return Vector3<_Ty>(
		vec.x % vec2.x,
		vec.y % vec2.y,
		vec.z % vec2.z
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector3<_Ty>& operator%(const Vector3<_Ty>& vec, const Vector3<_BTy>& vec2) {
	return Vector3<_Ty>(vec % Vector3<_Ty>(vec2));
}
template<class _Ty> NODISCARD CONSTEXPR Vector3<_Ty>& operator%(const Vector3<_Ty>& vec, const _Ty& scalar) {
	return Vector3<_Ty>(
		vec.x % scalar,
		vec.y % scalar,
		vec.z % scalar
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector3<_Ty>& operator%(const Vector3<_Ty>& vec, const _BTy& scalar) {
	return Vector3<_Ty>(
		vec.x % static_cast<_Ty>(scalar),
		vec.y % static_cast<_Ty>(scalar),
		vec.z % static_cast<_Ty>(scalar)
	);
}
template<class _Ty> NODISCARD CONSTEXPR Vector3<_Ty>& operator%(const _Ty& scalar, const Vector3<_Ty>& vec) {
	return Vector3<_Ty>(
		vec.x % scalar,
		vec.y % scalar,
		vec.z % scalar
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector3<_Ty>& operator%(const _BTy& scalar, const Vector3<_Ty>& vec) {
	return Vector3<_Ty>(
		vec.x % static_cast<_Ty>(scalar),
		vec.y % static_cast<_Ty>(scalar),
		vec.z % static_cast<_Ty>(scalar)
	);
}


/** Vector 4 **/

template<class _Ty> NODISCARD CONSTEXPR Vector4<_Ty>& operator+(const Vector4<_Ty>& vec, const Vector4<_Ty>& vec2) {
	return Vector4<_Ty>(
		vec.x + vec2.x,
		vec.y + vec2.y,
		vec.z + vec2.z,
		vec.w + vec2.w
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector4<_Ty>& operator+(const Vector4<_Ty>& vec, const Vector4<_BTy>& vec2) {
	return Vector4<_Ty>(vec + Vector4<_Ty>(vec2));
}
template<class _Ty> NODISCARD CONSTEXPR Vector4<_Ty>& operator+(const Vector4<_Ty>& vec, const _Ty& scalar) {
	return Vector4<_Ty>(
		vec.x + scalar,
		vec.y + scalar,
		vec.z + scalar,
		vec.w + scalar
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector4<_Ty>& operator+(const Vector4<_Ty>& vec, const _BTy& scalar) {
	return Vector4<_Ty>(
		vec.x + static_cast<_Ty>(scalar),
		vec.y + static_cast<_Ty>(scalar),
		vec.z + static_cast<_Ty>(scalar),
		vec.w + static_cast<_Ty>(scalar)
	);
}
template<class _Ty> NODISCARD CONSTEXPR Vector4<_Ty>& operator+(const _Ty& scalar, const Vector4<_Ty>& vec) {
	return Vector4<_Ty>(
		vec.x + scalar,
		vec.y + scalar,
		vec.z + scalar,
		vec.w + scalar
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector4<_Ty>& operator+(const _BTy& scalar, const Vector4<_Ty>& vec) {
	return Vector4<_Ty>(
		vec.x + static_cast<_Ty>(scalar),
		vec.y + static_cast<_Ty>(scalar),
		vec.z + static_cast<_Ty>(scalar),
		vec.w + static_cast<_Ty>(scalar)
	);
}

template<class _Ty> NODISCARD CONSTEXPR Vector4<_Ty>& operator-(const Vector4<_Ty>& vec, const Vector4<_Ty>& vec2) {
	return Vector4<_Ty>(
		vec.x - vec2.x,
		vec.y - vec2.y,
		vec.z - vec2.z,
		vec.w - vec2.w
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector4<_Ty>& operator-(const Vector4<_Ty>& vec, const Vector4<_BTy>& vec2) {
	return Vector4<_Ty>(vec - Vector4<_Ty>(vec2));
}
template<class _Ty> NODISCARD CONSTEXPR Vector4<_Ty>& operator-(const Vector4<_Ty>& vec, const _Ty& scalar) {
	return Vector4<_Ty>(
		vec.x - scalar,
		vec.y - scalar,
		vec.z - scalar,
		vec.w - scalar
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector4<_Ty>& operator-(const Vector4<_Ty>& vec, const _BTy& scalar) {
	return Vector4<_Ty>(
		vec.x - static_cast<_Ty>(scalar),
		vec.y - static_cast<_Ty>(scalar),
		vec.z - static_cast<_Ty>(scalar),
		vec.w - static_cast<_Ty>(scalar)
	);
}
template<class _Ty> NODISCARD CONSTEXPR Vector4<_Ty>& operator-(const _Ty& scalar, const Vector4<_Ty>& vec) {
	return Vector4<_Ty>(
		vec.x - scalar,
		vec.y - scalar,
		vec.z - scalar,
		vec.w - scalar
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector4<_Ty>& operator-(const _BTy& scalar, const Vector4<_Ty>& vec) {
	return Vector4<_Ty>(
		vec.x - static_cast<_Ty>(scalar),
		vec.y - static_cast<_Ty>(scalar),
		vec.z - static_cast<_Ty>(scalar),
		vec.w - static_cast<_Ty>(scalar)
	);
}

template<class _Ty> NODISCARD CONSTEXPR Vector4<_Ty>& operator*(const Vector4<_Ty>& vec, const Vector4<_Ty>& vec2) {
	return Vector4<_Ty>(
		vec.x * vec2.x,
		vec.y * vec2.y,
		vec.z * vec2.z,
		vec.w * vec2.w
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector4<_Ty>& operator*(const Vector4<_Ty>& vec, const Vector4<_BTy>& vec2) {
	return Vector4<_Ty>(vec * Vector4<_Ty>(vec2));
}
template<class _Ty> NODISCARD CONSTEXPR Vector4<_Ty>& operator*(const Vector4<_Ty>& vec, const _Ty& scalar) {
	return Vector4<_Ty>(
		vec.x * scalar,
		vec.y * scalar,
		vec.z * scalar,
		vec.w * scalar
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector4<_Ty>& operator*(const Vector4<_Ty>& vec, const _BTy& scalar) {
	return Vector4<_Ty>(
		vec.x * static_cast<_Ty>(scalar),
		vec.y * static_cast<_Ty>(scalar),
		vec.z * static_cast<_Ty>(scalar),
		vec.w * static_cast<_Ty>(scalar)
	);
}
template<class _Ty> NODISCARD CONSTEXPR Vector4<_Ty>& operator*(const _Ty& scalar, const Vector4<_Ty>& vec) {
	return Vector4<_Ty>(
		vec.x * scalar,
		vec.y * scalar,
		vec.z * scalar,
		vec.w * scalar
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector4<_Ty>& operator*(const _BTy& scalar, const Vector4<_Ty>& vec) {
	return Vector4<_Ty>(
		vec.x * static_cast<_Ty>(scalar),
		vec.y * static_cast<_Ty>(scalar),
		vec.z * static_cast<_Ty>(scalar),
		vec.w * static_cast<_Ty>(scalar)
	);
}

template<class _Ty> NODISCARD CONSTEXPR Vector4<_Ty>& operator/(const Vector4<_Ty>& vec, const Vector4<_Ty>& vec2) {
	return Vector4<_Ty>(
		vec.x / vec2.x,
		vec.y / vec2.y,
		vec.z / vec2.z,
		vec.w / vec2.w
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector4<_Ty>& operator/(const Vector4<_Ty>& vec, const Vector4<_BTy>& vec2) {
	return Vector4<_Ty>(vec / Vector4<_Ty>(vec2));
}
template<class _Ty> NODISCARD CONSTEXPR Vector4<_Ty>& operator/(const Vector4<_Ty>& vec, const _Ty& scalar) {
	return Vector4<_Ty>(
		vec.x / scalar,
		vec.y / scalar,
		vec.z / scalar,
		vec.w / scalar
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector4<_Ty>& operator/(const Vector4<_Ty>& vec, const _BTy& scalar) {
	return Vector4<_Ty>(
		vec.x / static_cast<_Ty>(scalar),
		vec.y / static_cast<_Ty>(scalar),
		vec.z / static_cast<_Ty>(scalar),
		vec.w / static_cast<_Ty>(scalar)
	);
}
template<class _Ty> NODISCARD CONSTEXPR Vector4<_Ty>& operator/(const _Ty& scalar, const Vector4<_Ty>& vec) {
	return Vector4<_Ty>(
		vec.x / scalar,
		vec.y / scalar,
		vec.z / scalar,
		vec.w / scalar
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector4<_Ty>& operator/(const _BTy& scalar, const Vector4<_Ty>& vec) {
	return Vector4<_Ty>(
		vec.x / static_cast<_Ty>(scalar),
		vec.y / static_cast<_Ty>(scalar),
		vec.z / static_cast<_Ty>(scalar),
		vec.w / static_cast<_Ty>(scalar)
	);
}

template<class _Ty> NODISCARD CONSTEXPR Vector4<_Ty>& operator%(const Vector4<_Ty>& vec, const Vector4<_Ty>& vec2) {
	return Vector4<_Ty>(
		vec.x % vec2.x,
		vec.y % vec2.y,
		vec.z % vec2.z,
		vec.w % vec2.w
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector4<_Ty>& operator%(const Vector4<_Ty>& vec, const Vector4<_BTy>& vec2) {
	return Vector4<_Ty>(vec % Vector4<_Ty>(vec2));
}
template<class _Ty> NODISCARD CONSTEXPR Vector4<_Ty>& operator%(const Vector4<_Ty>& vec, const _Ty& scalar) {
	return Vector4<_Ty>(
		vec.x % scalar,
		vec.y % scalar,
		vec.z % scalar,
		vec.w % scalar
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector4<_Ty>& operator%(const Vector4<_Ty>& vec, const _BTy& scalar) {
	return Vector4<_Ty>(
		vec.x % static_cast<_Ty>(scalar),
		vec.y % static_cast<_Ty>(scalar),
		vec.z % static_cast<_Ty>(scalar),
		vec.w % static_cast<_Ty>(scalar)
	);
}
template<class _Ty> NODISCARD CONSTEXPR Vector4<_Ty>& operator%(const _Ty& scalar, const Vector4<_Ty>& vec) {
	return Vector4<_Ty>(
		vec.x % scalar,
		vec.y % scalar,
		vec.z % scalar,
		vec.w % scalar
	);
}
template<class _Ty, class _BTy> NODISCARD CONSTEXPR Vector4<_Ty>& operator%(const _BTy& scalar, const Vector4<_Ty>& vec) {
	return Vector4<_Ty>(
		vec.x % static_cast<_Ty>(scalar),
		vec.y % static_cast<_Ty>(scalar),
		vec.z % static_cast<_Ty>(scalar),
		vec.w % static_cast<_Ty>(scalar)
	);
}

} // namespace lyra
