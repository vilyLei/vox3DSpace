#ifndef VOXOL_INTENT_ENUM_FLAGS_H
#define VOXOL_INTENT_ENUM_FLAGS_H

#include <type_traits>
#include <concepts>
#include <iostream>

namespace Voxol::Intent
{
namespace Flag
{

template <typename Enum>
concept EnumType = std::is_enum_v<Enum>;

template <EnumType E>
class EnumFlags
{
public:
    using Underlying = std::underlying_type_t<E>;

    constexpr EnumFlags() noexcept = default;
    constexpr EnumFlags(E e) noexcept :
        value_(static_cast<Underlying>(e)) {}
    constexpr EnumFlags(Underlying v) noexcept :
        value_(v) {}

    constexpr EnumFlags(const EnumFlags&) noexcept            = default;
    constexpr EnumFlags& operator=(const EnumFlags&) noexcept = default;

    constexpr EnumFlags operator|(Enum e) const noexcept
    {
        return EnumFlags(value_ | static_cast<Underlying>(e));
    }
    constexpr EnumFlags operator&(Enum e) const noexcept
    {
        return EnumFlags(value_ & static_cast<Underlying>(e));
    }
    constexpr EnumFlags operator^(Enum e) const noexcept
    {
        return EnumFlags(value_ ^ static_cast<Underlying>(e));
    }
    constexpr EnumFlags operator~() const noexcept
    {
        return EnumFlags(~value_);
    }

    constexpr EnumFlags& operator|=(Enum e) noexcept
    {
        value_ |= static_cast<Underlying>(e);
        return *this;
    }
    constexpr EnumFlags& operator&=(Enum e) noexcept
    {
        value_ &= static_cast<Underlying>(e);
        return *this;
    }
    constexpr EnumFlags& operator^=(Enum e) noexcept
    {
        value_ ^= static_cast<Underlying>(e);
        return *this;
    }

    constexpr EnumFlags operator|(EnumFlags other) const noexcept
    {
        return EnumFlags(value_ | other.value_);
    }
    constexpr EnumFlags operator&(EnumFlags other) const noexcept
    {
        return EnumFlags(value_ & other.value_);
    }
    constexpr EnumFlags operator^(EnumFlags other) const noexcept
    {
        return EnumFlags(value_ ^ other.value_);
    }
    constexpr EnumFlags& operator|=(EnumFlags other) noexcept
    {
        value_ |= other.value_;
        return *this;
    }
    constexpr EnumFlags& operator&=(EnumFlags other) noexcept
    {
        value_ &= other.value_;
        return *this;
    }
    constexpr EnumFlags& operator^=(EnumFlags other) noexcept
    {
        value_ ^= other.value_;
        return *this;
    }

    constexpr bool test(E e) const noexcept
    {
        return (value_ & static_cast<Underlying>(e)) == static_cast<Underlying>(e);
    }

    constexpr void set(E e, bool enabled = true) noexcept
    {
        if (enabled)
            value_ |= static_cast<Underlying>(e);
        else
            value_ &= ~static_cast<Underlying>(e);
    }

    constexpr void clear(E e) noexcept
    {
        value_ &= ~static_cast<Underlying>(e);
    }

    constexpr bool any() const noexcept { return value_ != 0; }
    constexpr bool none() const noexcept { return value_ == 0; }

    constexpr Underlying value() const noexcept { return value_; }

    constexpr bool operator==(EnumFlags other) const noexcept = default;

private:
    Underlying value_ = 0;
};

template <EnumType E>
constexpr EnumFlags<E> operator|(E lhs, E rhs) noexcept
{
    using T = std::underlying_type_t<E>;
    return EnumFlags<E>(static_cast<T>(lhs) | static_cast<T>(rhs));
}
} // namespace Flag

} // namespace Voxol::Intent
#endif