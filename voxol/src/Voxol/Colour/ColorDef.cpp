
#include "ColorDef.h"
#include <format>

namespace Voxol::Colour
{
namespace Component
{

std::string Color::toString(std::string_view fmt) const
{
    if (fmt == "hex")
    {
        return std::format("#{:02X}{:02X}{:02X}{:02X}", r(), g(), b(), a());
    }
    else if (fmt == "argb")
    {
        return std::format("ARGB({:02X}{:02X}{:02X}{:02X})", a(), r(), g(), b());
    }
    else if (fmt == "rgba")
    {
        return std::format("RGBA({:02X}{:02X}{:02X}{:02X})", r(), g(), b(), a());
    }
    else if (fmt == "css")
    {
        if (a() == 255)
        {
            return std::format("rgb({}, {}, {})", r(), g(), b());
        }
        else
        {
            return std::format("rgba({}, {}, {}, {:.2f})", r() / 255.0f, g() / 255.0f, b() / 255.0f, a() / 255.0f);
        }
    }
    else
    {
        return std::format("Color(r={}, g={}, b={}, a={})", r(), g(), b(), a());
    }
}
std::string Color::toHexString() const
{
    return std::format("0x{:02X}{:02X}{:02X}{:02X}", r(), g(), b(), a());
}
std::string Color::toCSSString() const
{
    return std::format("#{:02X}{:02X}{:02X}{:02X}", r(), g(), b(), a());
}

} // namespace Component
} // namespace Voxol::Colour