#ifndef VOXOL_INTENT_ENUM_WRAPPER_H
#define VOXOL_INTENT_ENUM_WRAPPER_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <concepts>
#include <type_traits>

namespace Voxol::Intent
{
namespace Flag
{


template <typename E>
concept EnumType = std::is_enum_v<E>;

template <EnumType E>
class EnumWrapper
{
public:
    using Underlying = std::underlying_type_t<E>;

    constexpr EnumWrapper() noexcept :
        mValue(0) {}
    constexpr EnumWrapper(E e) noexcept
        :
        mValue(static_cast<Underlying>(e)) {}
    constexpr EnumWrapper(Underlying v) noexcept
        :
        mValue(v) {}

    constexpr EnumWrapper operator|(E rhs) const noexcept
    {
        return EnumWrapper(mValue | static_cast<Underlying>(rhs));
    }
    constexpr EnumWrapper operator&(E rhs) const noexcept
    {
        return EnumWrapper(mValue & static_cast<Underlying>(rhs));
    }
    constexpr EnumWrapper operator^(E rhs) const noexcept
    {
        return EnumWrapper(mValue ^ static_cast<Underlying>(rhs));
    }
    constexpr EnumWrapper& operator|=(E rhs) noexcept
    {
        mValue |= static_cast<Underlying>(rhs);
        return *this;
    }
    constexpr EnumWrapper& operator&=(E rhs) noexcept
    {
        mValue &= static_cast<Underlying>(rhs);
        return *this;
    }
    constexpr EnumWrapper& operator^=(E rhs) noexcept
    {
        mValue ^= static_cast<Underlying>(rhs);
        return *this;
    }

    
    constexpr EnumWrapper operator|(EnumWrapper rhs) const noexcept
    {
        return EnumWrapper(mValue | rhs.mValue);
    }
    constexpr EnumWrapper operator&(EnumWrapper rhs) const noexcept
    {
        return EnumWrapper(mValue & rhs.mValue);
    }
    constexpr EnumWrapper operator^(EnumWrapper rhs) const noexcept
    {
        return EnumWrapper(mValue ^ rhs.mValue);
    }
    constexpr EnumWrapper operator~() const noexcept
    {
        return EnumWrapper(~mValue);
    }
    constexpr EnumWrapper& operator|=(EnumWrapper rhs) noexcept
    {
        mValue |= rhs.mValue;
        return *this;
    }
    constexpr EnumWrapper& operator&=(EnumWrapper rhs) noexcept
    {
        mValue &= rhs.mValue;
        return *this;
    }
    constexpr EnumWrapper& operator^=(EnumWrapper rhs) noexcept
    {
        mValue ^= rhs.mValue;
        return *this;
    }

    constexpr bool operator==(EnumWrapper rhs) const noexcept
    {
        return mValue == rhs.mValue;
    }
    constexpr bool operator!=(EnumWrapper rhs) const noexcept
    {
        return mValue != rhs.mValue;
    }

    constexpr bool test(E flag) const noexcept
    {
        return (mValue & static_cast<Underlying>(flag)) ==
            static_cast<Underlying>(flag);
    }

    constexpr void set(E flag, bool enabled = true) noexcept
    {
        if (enabled)
            mValue |= static_cast<Underlying>(flag);
        else
            mValue &= ~static_cast<Underlying>(flag);
    }

    constexpr void clear(E flag) noexcept
    {
        mValue &= ~static_cast<Underlying>(flag);
    }

    constexpr bool any() const noexcept { return mValue != 0; }
    constexpr bool none() const noexcept { return mValue == 0; }

    constexpr Underlying value() const noexcept { return mValue; }

private:
    Underlying mValue;
};


template <EnumType E>
constexpr EnumWrapper<E> operator|(E lhs, E rhs) noexcept
{
    using U = std::underlying_type_t<E>;
    return EnumWrapper<E>(static_cast<U>(lhs) | static_cast<U>(rhs));
}


} // namespace Flag

} // namespace Voxol::Intent
#endif