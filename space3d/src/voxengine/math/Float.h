#pragma once

/*******************************************************************************
* If the float number value is MATH_MAX_NEGATIVE < value < MATH_MIN_POSITIVE, 
* the value can be considered ZERO, otherwise the value is not ZERO.
*******************************************************************************/
namespace voxengine::math
{

/**
 * principle: x < MATH_MIN_POSITIVE, or x >= MATH_MIN_POSITIVE
 */
const double MATH_MIN_POSITIVE = 1e-6;
/**
 * principle: x <= MATH_MAX_NEGATIVE, or x > MATH_MAX_NEGATIVE
 */
const double MATH_MAX_NEGATIVE = -1e-6;

inline bool isZero(double v) {
	return v > MATH_MAX_NEGATIVE && v < MATH_MIN_POSITIVE;
}
inline bool isNotZero(double v) {
	return v <= MATH_MAX_NEGATIVE || v >= MATH_MIN_POSITIVE;
}
/**
 * example:
 *     isGreaterPositiveZero(0.1) is true
 *     isGreaterPositiveZero(0.000000001) is false
 *     isGreaterPositiveZero(-0.1) is false
 * @param v number value
 * @returns a positive number value and its value is greater zero, return true, otherwize false
 */
inline bool isGreaterPositiveZero(double v) {
	return v >= MATH_MIN_POSITIVE;
}
/**
 * example:
 *     isLessNegativeZero(-0.1) is true
 *     isLessNegativeZero(-0.000000001) is false
 *     isLessNegativeZero(0.1) is false
 * @param v number value
 * @returns a negative number value and its value is less zero, return true, otherwise false
 */
inline bool isLessNegativeZero(double v) {
	return v <= MATH_MAX_NEGATIVE;
}
/**
 * example:
 * 	isLessPositiveZero(+0.00000001) is true
 *  isLessPositiveZero(-1.3) is true
 *  isLessPositiveZero(1.3) is false
 * @param v number value
 * @returns true or false
 */
inline bool isLessPositiveZero(double v) {
	return v < MATH_MIN_POSITIVE;
}
/**
 * example:
 * 	isGreaterNegativeZero(-0.00000001) is true
 *  isGreaterNegativeZero(+1.3) is true
 *  isGreaterNegativeZero(-1.3) is false
 * @param v number value
 * @returns true or false
 */
inline bool isGreaterNegativeZero(double v) {
	return v > MATH_MAX_NEGATIVE;
}

inline bool isPostiveZero(double v) {
	return v >= 0.0 && v < MATH_MIN_POSITIVE;
}
inline bool isNegativeZero(double v) {
	return v <= 0.0 && v > MATH_MAX_NEGATIVE;
}
inline bool isGreaterRealZero(double v) {
	return v > 0.0;
}
inline bool isLessRealZero(double v) {
	return v < 0.0;
}
} // namespace voxengine::math