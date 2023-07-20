#include "Box.h"



namespace baseMath {


	Aabb::Corners Aabb::getCorners() const {
		return Aabb::Corners{ 
			   {
					{ min.x, min.y, min.z },
					{ max.x, min.y, min.z },
					{ min.x, max.y, min.z },
					{ max.x, max.y, min.z },
					{ min.x, min.y, max.z },
					{ max.x, min.y, max.z },
					{ min.x, max.y, max.z },
					{ max.x, max.y, max.z },
				} };
	}

	float Aabb::contains(Float3 p) const noexcept {
		float d = min.x - p.x;
		d = std::max(d, min.y - p.y);
		d = std::max(d, min.z - p.z);
		d = std::max(d, p.x - max.x);
		d = std::max(d, p.y - max.y);
		d = std::max(d, p.z - max.z);
		return d;
	}

	// Fast AABB transformation per Jim Arvo in Graphics Gems (1990).
	Aabb Aabb::transform(const mat4f& mat) const noexcept {
		const Float3 translation = mat[3].xyz;
		const mat3f upperLeft = mat.upperLeft();
		Aabb result = { translation, translation };
		for (size_t col = 0; col < upperLeft.NUM_COLS; ++col) {
			for (size_t row = 0; row < upperLeft.NUM_ROWS; ++row) {
				const float a = upperLeft[col][row] * min[col];
				const float b = upperLeft[col][row] * max[col];
				result.min[row] += a < b ? a : b;
				result.max[row] += a < b ? b : a;
			}
		}
		return result;
	}

} // namespace filament