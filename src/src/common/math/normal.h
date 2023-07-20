#pragma once
#include <cmath>
#include <stdint.h>
#include "scalar.h"
#include "vec4.h"	 


namespace baseMath
{
	namespace utils
	{

		inline uint16_t packUnorm16(float v) noexcept {
			return static_cast<uint16_t>(std::round(clamp(v, 0.0f, 1.0f) * 65535.0f));
		}

		inline Ushort4 packUnorm16(Float4 v) noexcept {
			return Ushort4{ packUnorm16(v.x), packUnorm16(v.y), packUnorm16(v.z), packUnorm16(v.w) };
		}

		inline int16_t packSnorm16(float v) noexcept {
			return static_cast<int16_t>(std::round(clamp(v, -1.0f, 1.0f) * 32767.0f));
		}

		inline Short2 packSnorm16(Float2 v) noexcept {
			return Short2{ packSnorm16(v.x), packSnorm16(v.y) };
		}

		inline Short4 packSnorm16(Float4 v) noexcept {
			return Short4{ packSnorm16(v.x), packSnorm16(v.y), packSnorm16(v.z), packSnorm16(v.w) };
		}

		inline float unpackUnorm16(uint16_t v) noexcept {
			return v / 65535.0f;
		}

		inline Float4 unpackUnorm16(Ushort4 v) noexcept {
			return Float4{ unpackUnorm16(v.x), unpackUnorm16(v.y), unpackUnorm16(v.z), unpackUnorm16(v.w) };
		}

		inline float unpackSnorm16(int16_t v) noexcept {
			return clamp(v / 32767.0f, -1.0f, 1.0f);
		}

		inline Float4 unpackSnorm16(Short4 v) noexcept {
			return Float4{ unpackSnorm16(v.x), unpackSnorm16(v.y), unpackSnorm16(v.z), unpackSnorm16(v.w) };
		}

		inline uint8_t packUnorm8(float v) noexcept {
			return static_cast<uint8_t>(std::round(clamp(v, 0.0f, 1.0f) * 255.0));
		}

		inline Ubyte4 packUnorm8(Float4 v) noexcept {
			return Ubyte4{ packUnorm8(v.x), packUnorm8(v.y), packUnorm8(v.z), packUnorm8(v.w) };
		}

		inline int8_t packSnorm8(float v) noexcept {
			return static_cast<int8_t>(std::round(clamp(v, -1.0f, 1.0f) * 127.0));
		}

		inline Byte4 packSnorm8(Float4 v) noexcept {
			return Byte4{ packSnorm8(v.x), packSnorm8(v.y), packSnorm8(v.z), packSnorm8(v.w) };
		}

		inline float unpackUnorm8(uint8_t v) noexcept {
			return v / 255.0f;
		}

		inline Float4 unpackUnorm8(Ubyte4 v) noexcept {
			return Float4{ unpackUnorm8(v.x), unpackUnorm8(v.y), unpackUnorm8(v.z), unpackUnorm8(v.w) };
		}

		inline float unpackSnorm8(int8_t v) noexcept {
			return clamp(v / 127.0f, -1.0f, 1.0f);
		}

		inline Float4 unpackSnorm8(Byte4 v) noexcept {
			return Float4{ unpackSnorm8(v.x), unpackSnorm8(v.y), unpackSnorm8(v.z), unpackSnorm8(v.w) };
		}

	} // namespace math
}