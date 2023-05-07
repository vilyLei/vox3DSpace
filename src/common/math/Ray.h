#pragma  once 
#include "commonDef.h"
#include <limits>
#include "vec3.h"
#include "mathutil.h"


namespace baseMath {

	class Box;
	class Sphere;
	class Plane;

	/** Representation of a ray in space, i.e. a line with an origin and direction. */
	class BaseExport Ray
	{
	protected:
		Float3 mOrigin;
		Float3 mDirection;
	public:
		Ray() :mOrigin(Float3::ZERO), mDirection(Float3::UNIT_Z) {}
		Ray(const Float3& origin, const Float3& direction)
			:mOrigin(origin), mDirection(direction) {}

		/** Sets the origin of the ray. */
		void setOrigin(const Float3& origin) { mOrigin = origin; }
		/** Gets the origin of the ray. */
		const Float3& getOrigin(void) const { return mOrigin; }

		/** Sets the direction of the ray. */
		void setDirection(const Float3& dir) { mDirection = dir; }
		/** Gets the direction of the ray. */
		const Float3& getDirection(void) const { return mDirection; }

		/** Gets the position of a point t units along the ray. */
		Float3 getPoint(float t) const {
			return Float3(mOrigin + (mDirection * t));
		}

		/** Gets the position of a point t units along the ray. */
		Float3 operator*(float t) const {
			return getPoint(t);
		}

		/** Tests whether this ray intersects the given plane.
		@return A pair structure where the first element indicates whether
			an intersection occurs, and if true, the second element will
			indicate the distance along the ray at which it intersects.
			This can be converted to a point in space by calling getPoint().
		*/
		std::pair<bool, float> intersects(const Plane& p) const
		{
			return Math::intersects(*this, p);
		}

		/** Tests whether this ray intersects the given sphere.
		@return A pair structure where the first element indicates whether
			an intersection occurs, and if true, the second element will
			indicate the distance along the ray at which it intersects.
			This can be converted to a point in space by calling getPoint().
		*/
		std::pair<bool, float> intersects(const Sphere& s) const
		{
			return Math::intersects(*this, s);
		}
		/** Tests whether this ray intersects the given box.
		@return A pair structure where the first element indicates whether
			an intersection occurs, and if true, the second element will
			indicate the distance along the ray at which it intersects.
			This can be converted to a point in space by calling getPoint().
		*/
		std::pair<bool, float> intersects(const Box& box) const
		{
			return Math::intersects(*this, box);
		}

	};
}