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

#ifndef MATH_VEC2_H_
#define MATH_VEC2_H_

#include "../stream/preCompileDef.h" 
#include "TVecHelpers.h"
#include "half.h"

namespace baseMath {


	template <typename T>
	class  MATH_EMPTY_BASES TVec2 :
		public TVecProductOperators<TVec2, T>,
		public TVecAddOperators<TVec2, T>,
		public TVecUnaryOperators<TVec2, T>,
		public TVecComparisonOperators<TVec2, T>,
		public TVecFunctions<TVec2, T>,
		public TVecDebug<TVec2, T> {
	public:
		typedef T value_type;
		typedef T& reference;
		typedef T const& const_reference;
		typedef size_t size_type;
		static constexpr size_t SIZE = 2;

		union {
			T v[SIZE];
			struct { T x, y; };
			struct { T s, t; };
			struct { T r, g; };
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
		constexpr TVec2() = default;

		// handles implicit conversion to a tvec4. must not be explicit.
		template<typename A>
		constexpr TVec2(A v) : x(v), y(v) { }

		template<typename A, typename B>
		constexpr TVec2(A x, B y) : x(x), y(y) { }

		template<typename A>
		constexpr TVec2(const TVec2<A>& v) : x(v.x), y(v.y) { }

		// cross product works only on vectors of size 2 or 3
		template <typename RT>
		friend inline
			constexpr value_type cross(const TVec2& u, const TVec2<RT>& v) {
			return value_type(u.x*v.y - u.y*v.x);
		}

		inline T squaredLength() const
		{
			return x * x + y * y ;
		}

		inline T Length() const {
			return std::sqrt(x * x + y * y );
		}
	};


	// ----------------------------------------------------------------------------------------

	template <typename T, typename = std::enable_if<std::is_arithmetic<T>::value >>
	using vec2 = TVec2<T>;

	using Double2 = vec2<double>;
	using Float2 = vec2<float>;
	using Half2 = vec2<half>;
	using Int2 = vec2<int32_t>;
	using Uint2 = vec2<uint32_t>;
	using Short2 = vec2<int16_t>;
	using Ushort2 = vec2<uint16_t>;
	using Byte2 = vec2<int8_t>;
	using Ubyte2 = vec2<uint8_t>;
	using Bool2 = vec2<bool>;

}  // namespace filament

#endif  // MATH_VEC2_H_
