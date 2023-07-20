#include "vec3.h"

namespace baseMath
{

#ifndef _WIN32
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
}