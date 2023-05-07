/*
 * Copyright 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MATH_VEC3_H_
#define MATH_VEC3_H_


#include "vec2.h"

#pragma  warning(disable: 4244)


namespace baseMath {

template <typename T>
class  MATH_EMPTY_BASES  TVec3 :
                public TVecProductOperators<TVec3, T>,
                public TVecAddOperators<TVec3, T>,
                public TVecUnaryOperators<TVec3, T>,
                public TVecComparisonOperators<TVec3, T>,
                public TVecFunctions<TVec3, T>,
                public TVecDebug<TVec3, T> {
public:
    typedef T value_type;
    typedef T& reference;
    typedef T const& const_reference;
    typedef size_t size_type;
    static constexpr size_t SIZE = 3;

    union {
        T v[SIZE];
        TVec2<T> xy;
        TVec2<T> st;
        TVec2<T> rg;
        struct {
            union {
                T x;
                T s;
                T r;
            };
            union {
                struct { T y, z; };
                struct { T t, p; };
                struct { T g, b; };
                TVec2<T> yz;
                TVec2<T> tp;
                TVec2<T> gb;
            };
        };
    };

    inline constexpr size_type size() const { return SIZE; }

    // array access
    inline constexpr T const& operator[](size_t i) const {
        // only possible in C++0x14 with constexpr
        assert(i < SIZE);
        return v[i];
    }

    inline constexpr T& operator[](size_t i) {
        assert(i < SIZE);
        return v[i];
    }

    // constructors

    // default constructor
    constexpr TVec3() = default;

    // handles implicit conversion to a tvec4. must not be explicit.
    template<typename A>
    constexpr TVec3(A v) : x(v), y(v), z(v) { }

    template<typename A, typename B, typename C>
    constexpr TVec3(A x, B y, C z) : x(x), y(y), z(z) { }

    template<typename A, typename B>
    constexpr TVec3(const TVec2<A>& v, B z) : x(v.x), y(v.y), z(z) { }

    template<typename A>
    constexpr TVec3(const TVec3<A>& v) : x(v.x), y(v.y), z(v.z) { }

    // cross product works only on vectors of size 3
    template <typename RT>
    friend inline
    constexpr TVec3 cross(const TVec3& u, const TVec3<RT>& v) {
        return TVec3(
                u.y*v.z - u.z*v.y,
                u.z*v.x - u.x*v.z,
                u.x*v.y - u.y*v.x);
    }

	inline T squaredLength() const
	{
		return x * x + y * y + z * z;
	}

	inline T Length() const {
		return std::sqrt(x * x + y * y + z * z);
	}

	/** Calculates the absolute dot (scalar) product of this vector with another.
	@remarks
		This function work similar dotProduct, except it use absolute value
		of each component of the vector to computing.
	@param
		vec Vector with which to calculate the absolute dot product (together
		with this one).
	@return
		A Real representing the absolute dot product value.
	*/
	inline float absDotProduct(const TVec3& vec) const
	{
		return fabs(x * vec.x) + fabs(y * vec.y) + fabs(z * vec.z);
	}

	inline float dotProduct(const TVec3& vec) const
	{
		return x * vec.x + y * vec.y + z * vec.z;
	}

	/** Returns true if the vector's scalar components are all greater
	that the ones of the vector it is compared against.
*/
	inline bool operator < (const TVec3& rhs) const
	{
		if (x < rhs.x && y < rhs.y && z < rhs.z)
			return true;
		return false;
	}

	/** Returns true if the vector's scalar components are all smaller
		that the ones of the vector it is compared against.
	*/
	inline bool operator > (const TVec3& rhs) const
	{
		if (x > rhs.x && y > rhs.y && z > rhs.z)
			return true;
		return false;
	}

	inline void swap(TVec3& other)
	{
		std::swap(x, other.x);
		std::swap(y, other.y);
		std::swap(z, other.z);
	}

public:
	/// special value, same as Vector3(0, 0, 0)
	static const TVec3 ZERO;
	/// special value, same as Vector3(-10000000, -10000000, -10000000)
	static const TVec3 INVALIDVAL;
	/// special value, same as Vector3(1, 0, 0)
	static const TVec3 UNIT_X;
	/// special value, same as Vector3(0, 1, 0)
	static const TVec3 UNIT_Y;
	/// special value, same as Vector3(0, 0, 1)
	static const TVec3 UNIT_Z;
	/// special value, same as Vector3(-1, 0, 0)
	static const TVec3 NEGATIVE_UNIT_X;
	/// special value, same as Vector3(0, -1, 0)
	static const TVec3 NEGATIVE_UNIT_Y;
	/// special value, same as Vector3(0, 0, -1)
	static const TVec3 NEGATIVE_UNIT_Z;
	/// special value, same as Vector3(1, 1, 1)
	static const TVec3 UNIT_SCALE;
};


// ----------------------------------------------------------------------------------------

template <typename T, typename =  std::enable_if<std::is_arithmetic<T>::value >>
using vec3 = TVec3<T>;

using Double3 = vec3<double>;
using Float3 = vec3<float>;
using Half3 = vec3<half>;
using Int3 = vec3<int32_t>;
using Uint3 = vec3<uint32_t>;
using Short3 = vec3<int16_t>;
using Ushort3 = vec3<uint16_t>;
using Byte3 = vec3<int8_t>;
using Ubyte3 = vec3<uint8_t>;
using Bool3 = vec3<bool>;  
using Int64Vec3 = vec3<int64_t>;

#ifdef _WIN32
// Float3
template<> const Float3 Float3::ZERO(0, 0, 0);
template<> const Float3 Float3::UNIT_X(1, 0, 0);
template<> const Float3 Float3::UNIT_Y(0, 1, 0);
template<> const Float3 Float3::UNIT_Z(0, 0, 1);
template<> const Float3 Float3::NEGATIVE_UNIT_X(-1, 0, 0);
template<> const Float3 Float3::NEGATIVE_UNIT_Y(0, -1, 0);
template<> const Float3 Float3::NEGATIVE_UNIT_Z(0, 0, -1);
template<> const Float3 Float3::UNIT_SCALE(1, 1, 1);

// Double3
template<> const Double3 Double3::ZERO(0, 0, 0);
template<> const Double3 Double3::UNIT_X(1, 0, 0);
template<> const Double3 Double3::UNIT_Y(0, 1, 0);
template<> const Double3 Double3::UNIT_Z(0, 0, 1);
template<> const Double3 Double3::NEGATIVE_UNIT_X(-1, 0, 0);
template<> const Double3 Double3::NEGATIVE_UNIT_Y(0, -1, 0);
template<> const Double3 Double3::NEGATIVE_UNIT_Z(0, 0, -1);
template<> const Double3 Double3::UNIT_SCALE(1, 1, 1);

// Int3
template<> const Int3 Int3::ZERO(0, 0, 0);
template<> const Int3 Int3::UNIT_X(1, 0, 0);
template<> const Int3 Int3::UNIT_Y(0, 1, 0);
template<> const Int3 Int3::UNIT_Z(0, 0, 1);
template<> const Int3 Int3::NEGATIVE_UNIT_X(-1, 0, 0);
template<> const Int3 Int3::NEGATIVE_UNIT_Y(0, -1, 0);
template<> const Int3 Int3::NEGATIVE_UNIT_Z(0, 0, -1);
template<> const Int3 Int3::UNIT_SCALE(1, 1, 1);
#endif
}  // namespace filament

#endif  // MATH_VEC3_H_
