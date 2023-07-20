#include "vec3.h"
#include "mathutil.h"
#include <limits>
#include "Ray.h"
#include "Plane.h"
#include "Sphere.h"
#include "Box.h"

namespace baseMath
{
	//-----------------------------------------------------------------------
bool Math::RealEqual(Real a, Real b, Real tolerance)
	{
		if (fabs(b - a) <= tolerance)
			return true;
		else
			return false;
	}

	bool Math::doubleEqual(double a, double b, double tolerance)
	{
		return (fabs(b - a) <= tolerance);
	}

	//-----------------------------------------------------------------------
	std::pair<bool, float> Math::intersects(const Ray& ray, const Plane& plane)
	{
		float denom = dot(plane.normal, ray.getDirection());
		if (Math::Abs(denom) < std::numeric_limits<float>::epsilon())
		{
			// Parallel
			return std::pair<bool, float>(false, (float)0);
		}
		else
		{
			float nom = dot(plane.normal, ray.getOrigin()) + plane.d;
			float t = -(nom / denom);
			return std::pair<bool, float>(t >= 0, (float)t);
		}
	}

	//-----------------------------------------------------------------------
	std::pair<bool, float> Math::intersects(const Ray& ray, const Sphere& sphere,
		bool discardInside)
	{
		const Float3& raydir = ray.getDirection();
		// Adjust ray origin relative to sphere center
		const Float3& rayorig = ray.getOrigin() - sphere.getCenter();
		float radius = sphere.getRadius();

		// Check origin inside first
		if (rayorig.squaredLength() <= radius * radius && discardInside)
		{
			return std::pair<bool, float>(true, (float)0);
		}

		// Mmm, quadratics
		// Build coeffs which can be used with std quadratic solver
		// ie t = (-b +/- sqrt(b*b + 4ac)) / 2a
		float a = raydir.dotProduct(raydir);
		float b = 2 * rayorig.dotProduct(raydir);
		float c = rayorig.dotProduct(rayorig) - radius * radius;

		// Calc determinant
		float d = (b*b) - (4 * a * c);
		if (d < 0)
		{
			// No intersection
			return std::pair<bool, float>(false, (float)0);
		}
		else
		{
			// BTW, if d=0 there is one intersection, if d > 0 there are 2
			// But we only want the closest one, so that's ok, just use the 
			// '-' version of the solver
			float t = (-b - Math::Sqrt(d)) / (2 * a);
			if (t < 0)
				t = (-b + Math::Sqrt(d)) / (2 * a);
			return std::pair<bool, float>(true, (float)t);
		}
	}

	//-----------------------------------------------------------------------
	std::pair<bool, float> Math::intersects(const Ray& ray, const Box& box)
	{
		float lowt = 0.0f;
		float t;
		bool hit = false;
		Float3 hitpoint;
		const Float3& min = box.getMin();
		const Float3& max = box.getMax();
		const Float3& rayorig = ray.getOrigin();
		const Float3& raydir = ray.getDirection();

		// Check origin inside first
		if (rayorig > min && rayorig < max)
		{
			return std::pair<bool, float>(true, (float)0);
		}

		// Check each face in turn, only check closest 3
		// Min x
		if (rayorig.x <= min.x && raydir.x > 0)
		{
			t = (min.x - rayorig.x) / raydir.x;
			if (t >= 0)
			{
				// Substitute t back into ray and check bounds and dist
				hitpoint = rayorig + raydir * t;
				if (hitpoint.y >= min.y && hitpoint.y <= max.y &&
					hitpoint.z >= min.z && hitpoint.z <= max.z &&
					(!hit || t < lowt))
				{
					hit = true;
					lowt = t;
				}
			}
		}
		// Max x
		if (rayorig.x >= max.x && raydir.x < 0)
		{
			t = (max.x - rayorig.x) / raydir.x;
			if (t >= 0)
			{
				// Substitute t back into ray and check bounds and dist
				hitpoint = rayorig + raydir * t;
				if (hitpoint.y >= min.y && hitpoint.y <= max.y &&
					hitpoint.z >= min.z && hitpoint.z <= max.z &&
					(!hit || t < lowt))
				{
					hit = true;
					lowt = t;
				}
			}
		}
		// Min y
		if (rayorig.y <= min.y && raydir.y > 0)
		{
			t = (min.y - rayorig.y) / raydir.y;
			if (t >= 0)
			{
				// Substitute t back into ray and check bounds and dist
				hitpoint = rayorig + raydir * t;
				if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
					hitpoint.z >= min.z && hitpoint.z <= max.z &&
					(!hit || t < lowt))
				{
					hit = true;
					lowt = t;
				}
			}
		}
		// Max y
		if (rayorig.y >= max.y && raydir.y < 0)
		{
			t = (max.y - rayorig.y) / raydir.y;
			if (t >= 0)
			{
				// Substitute t back into ray and check bounds and dist
				hitpoint = rayorig + raydir * t;
				if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
					hitpoint.z >= min.z && hitpoint.z <= max.z &&
					(!hit || t < lowt))
				{
					hit = true;
					lowt = t;
				}
			}
		}
		// Min z
		if (rayorig.z <= min.z && raydir.z > 0)
		{
			t = (min.z - rayorig.z) / raydir.z;
			if (t >= 0)
			{
				// Substitute t back into ray and check bounds and dist
				hitpoint = rayorig + raydir * t;
				if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
					hitpoint.y >= min.y && hitpoint.y <= max.y &&
					(!hit || t < lowt))
				{
					hit = true;
					lowt = t;
				}
			}
		}
		// Max z
		if (rayorig.z >= max.z && raydir.z < 0)
		{
			t = (max.z - rayorig.z) / raydir.z;
			if (t >= 0)
			{
				// Substitute t back into ray and check bounds and dist
				hitpoint = rayorig + raydir * t;
				if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
					hitpoint.y >= min.y && hitpoint.y <= max.y &&
					(!hit || t < lowt))
				{
					hit = true;
					lowt = t;
				}
			}
		}

		return std::pair<bool, float>(hit, (float)lowt);

	}


	//-----------------------------------------------------------------------
	bool Math::intersects(const Sphere& sphere, const Plane& plane)
	{
		return (
			Math::Abs(plane.getDistance(sphere.getCenter()))
			<= sphere.getRadius());
	}

	//-----------------------------------------------------------------------
	bool Math::intersects(const Sphere& sphere, const Box& box)
	{
		// Use splitting planes
		const Float3& center = sphere.getCenter();
		float radius = sphere.getRadius();
		const Float3& min = box.getMin();
		const Float3& max = box.getMax();

		// Arvo's algorithm
		float s, d = 0;
		for (int i = 0; i < 3; ++i)
		{
			if (center[i] < min[i])
			{
				s = center[i] - min[i];
				d += s * s;
			}
			else if (center[i] > max[i])
			{
				s = center[i] - max[i];
				d += s * s;
			}
		}
		return d <= radius * radius;
	}

	//-----------------------------------------------------------------------
	bool Math::intersects(const Plane& plane, const Box& box)
	{
		return (plane.getSide(box) == Plane::BOTH_SIDE);
	}

	//-----------------------------------------------------------------------
	std::pair<bool, float> Math::intersects(const Ray& ray, const Float3& a, const Float3& b, const Float3& c, const Float3& normal, bool positiveSide, bool negativeSide)
	{
		//
		// Calculate intersection with plane.
		//
		float t;
		{
			float denom = normal.dotProduct(ray.getDirection());

			// Check intersect side
			if (denom > +std::numeric_limits<float>::epsilon())
			{
				if (!negativeSide)
					return std::pair<bool, float>(false, (float)0);
			}
			else if (denom < -std::numeric_limits<float>::epsilon())
			{
				if (!positiveSide)
					return std::pair<bool, float>(false, (float)0);
			}
			else
			{
				// Parallel or triangle area is close to zero when
				// the plane normal not normalised.
				return std::pair<bool, float>(false, (float)0);
			}

			t = normal.dotProduct(a - ray.getOrigin()) / denom;

			if (t < 0)
			{
				// Intersection is behind origin
				return std::pair<bool, float>(false, (float)0);
			}
		}

		//
		// Calculate the largest area projection plane in X, Y or Z.
		//
		size_t i0, i1;
		{
			float n0 = Math::Abs(normal[0]);
			float n1 = Math::Abs(normal[1]);
			float n2 = Math::Abs(normal[2]);

			i0 = 1; i1 = 2;
			if (n1 > n2)
			{
				if (n1 > n0) i0 = 0;
			}
			else
			{
				if (n2 > n0) i1 = 0;
			}
		}

		//
		// Check the intersection point is inside the triangle.
		//
		{
			float u1 = b[i0] - a[i0];
			float v1 = b[i1] - a[i1];
			float u2 = c[i0] - a[i0];
			float v2 = c[i1] - a[i1];
			float u0 = t * ray.getDirection()[i0] + ray.getOrigin()[i0] - a[i0];
			float v0 = t * ray.getDirection()[i1] + ray.getOrigin()[i1] - a[i1];

			float alpha = u0 * v2 - u2 * v0;
			float beta = u1 * v0 - u0 * v1;
			float area = u1 * v2 - u2 * v1;

			// epsilon to avoid float precision error
			const float EPSILON = 1e-6f;

			float tolerance = -EPSILON * area;

			if (area > 0)
			{
				if (alpha < tolerance || beta < tolerance || alpha + beta > area - tolerance)
					return std::pair<bool, float>(false, (float)0);
			}
			else
			{
				if (alpha > tolerance || beta > tolerance || alpha + beta < area - tolerance)
					return std::pair<bool, float>(false, (float)0);
			}
		}

		return std::pair<bool, float>(true, (float)t);
	}

	//-----------------------------------------------------------------------
	std::pair<bool, float> Math::intersects(const Ray& ray, const Float3& a, const Float3& b, const Float3& c, bool positiveSide, bool negativeSide)
	{
		Float3 normal = calculateBasicFaceNormalWithoutNormalize(a, b, c);
		return intersects(ray, a, b, c, normal, positiveSide, negativeSide);
	}

	//-----------------------------------------------------------------------
	Float3 Math::calculateBasicFaceNormalWithoutNormalize(const Float3& v1, const Float3& v2, const Float3& v3)
	{
		Float3 normal =  cross((v2 - v1), (v3 - v1));
		return normal;
	}
}