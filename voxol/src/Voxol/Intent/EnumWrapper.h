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
        value_(0) {}
    constexpr EnumWrapper(E e) noexcept :
        value_(static_cast<Underlying>(e)) {}
    constexpr EnumWrapper(Underlying v) noexcept :
        value_(v) {}

    constexpr EnumWrapper  operator|(E rhs) const noexcept { return EnumWrapper(value_ | static_cast<Underlying>(rhs)); }
    constexpr EnumWrapper& operator|=(E rhs) noexcept
    {
        value_ |= static_cast<Underlying>(rhs);
        return *this;
    }

    constexpr EnumWrapper  operator&(E rhs) const noexcept { return EnumWrapper(value_ & static_cast<Underlying>(rhs)); }
    constexpr EnumWrapper& operator&=(E rhs) noexcept
    {
        value_ &= static_cast<Underlying>(rhs);
        return *this;
    }

    constexpr EnumWrapper operator~() const noexcept { return EnumWrapper(~value_); }

    constexpr bool test(E flag) const noexcept { return (value_ & static_cast<Underlying>(flag)) == static_cast<Underlying>(flag); }
    constexpr void set(E flag, bool enabled = true) noexcept
    {
        if (enabled) value_ |= static_cast<Underlying>(flag);
        else
            value_ &= ~static_cast<Underlying>(flag);
    }
    constexpr void clear(E flag) noexcept { value_ &= ~static_cast<Underlying>(flag); }

    constexpr bool any() const noexcept { return value_ != 0; }
    constexpr bool none() const noexcept { return value_ == 0; }

    constexpr Underlying value() const noexcept { return value_; }

    std::string toString(const std::vector<std::pair<E, std::string>>& nameMap) const
    {
        if (value_ == 0) return "None";

        std::ostringstream oss;
        bool               first = true;
        for (auto& pair : nameMap)
        {
            if (test(pair.first))
            {
                if (!first) oss << " | ";
                oss << pair.second;
                first = false;
            }
        }
        return oss.str();
    }

private:
    Underlying value_;
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