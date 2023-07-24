#pragma  once 
#include <limits> 
#include "vec3.h"
#include "Box.h"

namespace baseMath
{

	/** Defines a plane in 3D space.
	@remarks
		A plane is defined in 3D space by the equation
		Ax + By + Cz + D = 0
	@par
		This equates to a vector (the normal of the plane, whose x, y
		and z components equate to the coefficients A, B and C
		respectively), and a constant (D) which is the distance along
		the normal you have to go to move the plane back to the origin.
    */
	class BaseExport Plane
	{
	public:
		/// Default constructor - sets everything to 0.
		Plane();
		/// Constructor with another Plane.
		Plane(const Plane& rhs);
		/// Construct a plane through a normal, and a distance to move the plane along the normal.
		Plane(const Float3& rkNormal, float fConstant);
		/// Construct a plane using the 4 constants directly.
		Plane(float a, float b, float c, float d);
		/// Construct a plane using rkNormal and rkPoint.
		Plane(const Float3& rkNormal, const Float3& rkPoint);
		/// Construct a plane using three points.
		Plane(const Float3& rkPoint0, const Float3& rkPoint1, const Float3& rkPoint2);

		/** The "positive side" of the plane is the half space to which the
			plane normal points. The "negative side" is the other half
			space. The flag "no side" indicates the plane itself.
		*/
		enum Side
		{
			NO_SIDE,
			POSITIVE_SIDE,
			NEGATIVE_SIDE,
			BOTH_SIDE
		};

		/// Get the side of the given point.
		Side getSide(const Float3& rkPoint) const;

		/**
		Returns the side where the alignedBox is. The flag BOTH_SIDE indicates an intersecting box.
		One corner ON the plane is sufficient to consider the box and the plane intersecting.
		*/
		Side getSide(const Box& rkBox) const;

		/** Returns which side of the plane that the given box lies on.
			The box is defined as centre/half-size pairs for effectively.
		@param centre The centre of the box.
		@param halfSize The half-size of the box.
		@return
			POSITIVE_SIDE if the box complete lies on the "positive side" of the plane,
			NEGATIVE_SIDE if the box complete lies on the "negative side" of the plane,
			and BOTH_SIDE if the box intersects the plane.
		*/
		Side getSide(const Float3& centre, const Float3& halfSize) const;

		/** This is a pseudo distance. The sign of the return value is
			positive if the point is on the positive side of the plane,
			negative if the point is on the negative side, and zero if the
			point is on the plane.
			@par
			The absolute value of the return value is the true distance only
			when the plane normal is a unit length vector.
		*/
		float getDistance(const Float3& rkPoint) const;

		/** Redefine this plane based on 3 points. */
		void redefine(const Float3& rkPoint0, const Float3& rkPoint1,
			const Float3& rkPoint2);

		/** Redefine this plane based on a normal and a point. */
		void redefine(const Float3& rkNormal, const Float3& rkPoint);

		/** Project a vector onto the plane.
		@remarks This gives you the element of the input vector that is perpendicular
			to the normal of the plane. You can get the element which is parallel
			to the normal of the plane by subtracting the result of this method
			from the original vector, since parallel + perpendicular = original.
		@param v The input vector
		*/
		Float3 projectVector(const Float3& v) const;

		/** normalizes the plane.
			@remarks
				This method normalizes the plane's normal and the length scale of d
				is as well.
			@note
				This function will not crash for zero-sized vectors, but there
				will be no changes made to their components.
			@return The previous length of the plane's normal.
		*/
		float normalise(void);

		Float3 normal;
		float d;

		/// Comparison operator for equality.
		bool operator==(const Plane& rhs) const
		{
			return (rhs.d == d && rhs.normal == normal);
		}

		/// Comparison operator for inequality.
		bool operator!=(const Plane& rhs) const
		{
			return (rhs.d != d || rhs.normal != normal);
		} 
	};

	typedef std::vector<Plane> PlaneList;

}