#pragma  once 
#include "../stream/commonDef.h"
#include <utility>
#include "mdefine.h"
#include "vec3.h"
#include "vec4.h"


//------------------------------------------------------------------------------------
// math utils function;
//------------------------------------------------------------------------------------

namespace baseMath
{
	namespace utils
	{
		namespace details {
			template<typename T>
			constexpr inline T popcount(T v) noexcept {
				static_assert(sizeof(T) * CHAR_BIT <= 128, "details::popcount() only support up to 128 bits");
				constexpr T ONES = ~T(0);
				v = v - ((v >> 1u) & ONES / 3);
				v = (v & ONES / 15 * 3) + ((v >> 2u) & ONES / 15 * 3);
				v = (v + (v >> 4u)) & ONES / 255 * 15;
				return (T)(v * (ONES / 255)) >> (sizeof(T) - 1) * CHAR_BIT;
			}

			template<typename T, typename = std::enable_if_t<std::is_unsigned<T>::value>>
			constexpr inline T clz(T x) noexcept {
				static_assert(sizeof(T) * CHAR_BIT <= 128, "details::clz() only support up to 128 bits");
				x |= (x >> 1u);
				x |= (x >> 2u);
				x |= (x >> 4u);
				x |= (x >> 8u);
				x |= (x >> 16u);
				if (sizeof(T) * CHAR_BIT >= 64) {   // just to silence compiler warning
					x |= (x >> 32u);
				}
				if (sizeof(T) * CHAR_BIT >= 128) {   // just to silence compiler warning
					x |= (x >> 64u);
				}
				return T(sizeof(T) * CHAR_BIT) - details::popcount(x);
			}

			template<typename T, typename = std::enable_if_t<std::is_unsigned<T>::value>>
			constexpr inline T ctz(T x) noexcept {
				static_assert(sizeof(T) * CHAR_BIT <= 64, "details::ctz() only support up to 64 bits");
				T c = sizeof(T) * CHAR_BIT;
				x &= -x;    // equivalent to x & (~x + 1)
				if (x) c--;
				if (sizeof(T) * CHAR_BIT >= 64) {
					if (x & T(0x00000000FFFFFFFF)) c -= 32;
				}
				if (x & T(0x0000FFFF0000FFFF)) c -= 16;
				if (x & T(0x00FF00FF00FF00FF)) c -= 8;
				if (x & T(0x0F0F0F0F0F0F0F0F)) c -= 4;
				if (x & T(0x3333333333333333)) c -= 2;
				if (x & T(0x5555555555555555)) c -= 1;
				return c;
			}
		}




		template<typename T>
		inline constexpr T  saturate(T v) noexcept {
			return T(std::min(T(1), std::max(T(0), v)));
		}

		template<typename T>
		inline constexpr T  clamp(T v, T min, T max) noexcept {
			return T(std::min(max, std::max(min, v)));
		}

		template<typename T>
		inline constexpr T  mix(T x, T y, T a) noexcept {
			return x * (T(1) - a) + y * a;
		}

		template<typename T>
		inline constexpr T  lerp(T x, T y, T a) noexcept {
			return mix(x, y, a);
		}

		template <typename T>
		inline constexpr T sign(T x) noexcept {
			return x < T(0) ? T(-1) : T(1);
		}
	}

	class Box;
	class Sphere;
	class Ray;
	class Plane;

	class Math
	{
	public:
		/** Absolute value function
		@param
			fValue The value whose absolute value will be returned.
	    */
		static inline float Abs(float fValue) { return float(fabs(fValue)); }
		/** Squared function.
		@param fValue
			The value to be squared (fValue^2)
		*/
		static inline float Sqr(float fValue) { return fValue * fValue; }

		/** Square root function.
			@param fValue
				The value whose square root will be calculated.
		 */
		static inline float Sqrt(float fValue) { return float(std::sqrt(fValue)); }

		/** Compare 2 reals, using tolerance for inaccuracies.
		*/
		static bool RealEqual(float a, float b, float tolerance = std::numeric_limits<float>::epsilon());
		static bool doubleEqual(double a, double b, double tolerance = std::numeric_limits<double>::epsilon());

		/** Ray / plane intersection, returns boolean result and distance. */
		static std::pair<bool, float> intersects(const Ray& ray, const Plane& plane);

		/** Ray / sphere intersection, returns boolean result and distance. */
		static std::pair<bool, float> intersects(const Ray& ray, const Sphere& sphere,
			bool discardInside = true);

		/** Ray / box intersection, returns boolean result and distance. */
		static std::pair<bool, float> intersects(const Ray& ray, const Box& box);

		/** Sphere / box intersection test. */
		static bool intersects(const Sphere& sphere, const Box& box);

		/** Plane / box intersection test. */
		static bool intersects(const Plane& plane, const Box& box);

		/** Sphere / plane intersection test.
		@remarks NB just do a plane.getDistance(sphere.getCenter()) for more detail!
		*/
		static bool intersects(const Sphere& sphere, const Plane& plane);

		static std::pair<bool, float> intersects(const Ray& ray, const Float3& a,
			const Float3& b, const Float3& c,
			const Float3& normal,
			bool positiveSide, bool negativeSide);

		static std::pair<bool, float> intersects(const Ray& ray, const Float3& a,
			const Float3& b, const Float3& c,
			bool positiveSide = true, bool negativeSide = true);

		static Float3 calculateBasicFaceNormalWithoutNormalize(const Float3& v1, const Float3& v2, const Float3& v3);
	};


	// sphere radius must be squared
	// plane equation must be normalized, sphere radius must be squared
	// return float4.w <= 0 if no intersection
	inline Float4 spherePlaneIntersection(Float4 s, Float4 p)  {
		const float d = dot(s.xyz, p.xyz) + p.w;
		const float rr = s.w - d * d;
		s.x -= p.x * d;
		s.y -= p.y * d;
		s.z -= p.z * d;
		s.w = rr;   // new-circle/sphere radius is squared
		return s;
	}

	// sphere radius must be squared
	// plane equation must be normalized and have the form {x,0,z,0}, sphere radius must be squared
	// return float4.w <= 0 if no intersection
	inline float spherePlaneDistanceSquared(Float4 s, float px, float pz)  {
		return spherePlaneIntersection(s, { px, 0.f, pz, 0.f }).w;
	}

	// sphere radius must be squared
	// plane equation must be normalized and have the form {0,y,z,0}, sphere radius must be squared
	// return float4.w <= 0 if no intersection
	inline Float4 spherePlaneIntersection(Float4 s, float py, float pz)  {
		return spherePlaneIntersection(s, { 0.f, py, pz, 0.f });
	}

	// sphere radius must be squared
	// plane equation must be normalized and have the form {0,0,1,w}, sphere radius must be squared
	// return float4.w <= 0 if no intersection
	inline Float4 spherePlaneIntersection(Float4 s, float pw)  {
		return spherePlaneIntersection(s, { 0.f, 0.f, 1.f, pw });
	}

	// sphere radius must be squared
	// this version returns a false-positive intersection in a small area near the origin
	// of the cone extended outward by the sphere's radius.
	inline bool sphereConeIntersectionFast(
		Float4 const& sphere,
		Float3 const& conePosition,
		Float3 const& coneAxis,
		float coneSinInverse,
		float coneCosSquared) 
	{
		const Float3 u = conePosition - (sphere.w * coneSinInverse) * coneAxis;
		Float3 d = sphere.xyz - u;
		float e = dot(coneAxis, d);
		float dd = dot(d, d);
		// we do the e>0 last here to avoid a branch
		return (e * e >= dd * coneCosSquared && e > 0);
	}

	inline bool sphereConeIntersection(
		Float4 const& sphere,
		Float3 const& conePosition,
		Float3 const& coneAxis,
		float coneSinInverse,
		float coneCosSquared) 
	{
		if (sphereConeIntersectionFast(sphere,
			conePosition, coneAxis, coneSinInverse, coneCosSquared)) {
			Float3 d = sphere.xyz - conePosition;
			float e = -dot(coneAxis, d);
			float dd = dot(d, d);
			if (e * e >= dd * (1 - coneCosSquared) && e > 0) {
				return dd <= sphere.w * sphere.w;
			}
			return true;
		}
		return false;
	}

	template<typename T,
		typename = std::enable_if_t<std::is_integral<T>::value && std::is_unsigned<T>::value>>
		constexpr inline  T log2i(T x) noexcept {
		return (sizeof(x) * 8 - 1u) - utils::details::clz(x);
	}
}
//------------------------------------------------------------------------------------