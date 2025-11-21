#ifndef VOXOL_SCENE_DESC_JSON_VALUE_H
#define VOXOL_SCENE_DESC_JSON_VALUE_H

#include <variant>
#include <nlohmann/json.hpp>
#include "../Math/Vec2.h"
#include "../Math/Vec3.h"
#include "../Math/Vec4.h"

namespace Voxol::Scene
{
namespace Data
{
using JsonType = nlohmann::json;

//struct ColorValue
//{
//    uint32_t color = 0x0;
//    bool     parseStringColor(const JsonType& node);
//    bool     parseStringColorWithName(const JsonType& node, const std::string& valueName);
//
//};
struct ColorValue
{
    // stored as ARGB (0xAARRGGBB)
    uint32_t color = 0x00000000;

    bool parse(const JsonType& node)
    {
        return parseWithName(node, "color") ||
            parseWithName(node, "fillColor") ||
            parseWithName(node, "strokeColor");
    }

    bool parseWithName(const JsonType& node, const std::string& key)
    {
        if (!node.contains(key))
            return false;

        const auto& v = node[key];

        if (v.is_number())
        {
            color = static_cast<uint32_t>(v);
            return true;
        }

        if (v.is_array())
        {
            return parseColorArray(v);
        }

        if (v.is_string())
        {
            return parseColorString(v.get<std::string>());
        }

        return false;
    }

private:
    // ----------------------------------------
    // parse ["r","g","b"] or ["r","g","b","a"]
    // float[0-1],uint8[0-255]
    // ----------------------------------------
    bool parseColorArray(const JsonType& arr)
    {
        if (arr.size() < 3 || arr.size() > 4)
            return false;

        float r = arr[0];
        float g = arr[1];
        float b = arr[2];
        float a = arr.size() == 4 ? (float)arr[3] : 1.0f;

        // auto-detect normalized floats vs 0-255
        auto conv = [](float x) -> uint8_t {
            if (x <= 1.0f) return (uint8_t)(x * 255.0f);
            return (uint8_t)x;
        };

        uint8_t R = conv(r);
        uint8_t G = conv(g);
        uint8_t B = conv(b);
        uint8_t A = conv(a);

        color = (A << 24) | (R << 16) | (G << 8) | B;
        return true;
    }

    // ----------------------------------------
    // parse string "#RRGGBB", "0xff00ff", "ff00ff", "red"
    // ----------------------------------------
    bool parseColorString(std::string s)
    {
        std::string str = s;

        // trim
        str.erase(0, str.find_first_not_of(" \t\n\r"));
        str.erase(str.find_last_not_of(" \t\n\r") + 1);

        // lower case
        std::transform(str.begin(), str.end(), str.begin(),
                       [](unsigned char c) { return std::tolower(c); });

        // hex: #xxxxxx
        if (str[0] == '#')
        {
            return parseHex(str.substr(1));
        }

        // hex: 0xFFFFFF
        if (str.rfind("0x", 0) == 0)
        {
            return parseHex(str.substr(2));
        }

        // hex: FFFFFF
        if (isHexString(str))
        {
            return parseHex(str);
        }

        // css color names
        return parseCssName(str);
    }

    bool isHexString(const std::string& s)
    {
        return std::all_of(s.begin(), s.end(), [](char c) {
            return std::isxdigit(c);
        });
    }

    // ----------------------------------------
    // RRGGBB / RRGGBBAA
    // ----------------------------------------
    bool parseHex(const std::string& hex)
    {
        uint32_t v = std::stoul(hex, nullptr, 16);

        if (hex.size() == 6)
        {
            // RGB ¡ú ARGB
            color = 0xFF000000 | v;
        }
        else if (hex.size() == 8)
        {
            // RGBA ¡ú ARGB
            //uint32_t r = (v >> 24) & 0xFF;
            //uint32_t g = (v >> 16) & 0xFF;
            //uint32_t b = (v >> 8) & 0xFF;
            //uint32_t a = (v >> 0) & 0xFF;
            //color      = (a << 24) | (r << 16) | (g << 8) | b;
            color = v;
        }
        else
        {
            return false;
        }
        return true;
    }

    bool parseCssName(const std::string& name)
    {
        static std::unordered_map<std::string, uint32_t> cssColors = {
            {"red", 0xFFFF0000},
            {"green", 0xFF00FF00},
            {"blue", 0xFF0000FF},
            {"white", 0xFFFFFFFF},
            {"black", 0xFF000000},
            {"yellow", 0xFFFFFF00},
            {"cyan", 0xFF00FFFF},
            {"magenta", 0xFFFF00FF},
            {"gray", 0xFF808080},
            {"grey", 0xFF808080},
        };

        auto it = cssColors.find(name);
        if (it != cssColors.end())
        {
            color = it->second;
            return true;
        }
        return false;
    }
};


struct JsonValue
{
    enum class Type : uint8_t
    {
        Null,
        Bool,
        Int,
        Uint32,
        Float,
        String,
        Vec2,
        Vec3,
        Vec4,
        Array,
        Object
    };

    using ArrayType  = std::vector<JsonValue>;
    using ObjectType = std::unordered_map<std::string, JsonValue>;

    using Variant = std::variant<
        std::monostate, // Null
        bool,
        int,
        uint32_t,
        float,
        std::string,
        Math::Vec2,
        Math::Vec3,
        Math::Vec4,
        ArrayType,
        ObjectType>;

    Type    type = Type::Null;
    Variant value;

    JsonValue() = default;

    JsonValue(float v) :
        type(Type::Float), value(v) {}
    JsonValue(int v) :
        type(Type::Int), value(v) {}
    JsonValue(uint32_t v) :
        type(Type::Uint32), value(v) {}
    JsonValue(bool v) :
        type(Type::Bool), value(v) {}
    JsonValue(const std::string& s) :
        type(Type::String), value(s) {}
    JsonValue(const char* s) :
        type(Type::String), value(std::string(s)) {}

    JsonValue(const Math::Vec2& v2) :
        type(Type::Vec2), value(v2) {}
    JsonValue(const Math::Vec3& v3) :
        type(Type::Vec3), value(v3) {}
    JsonValue(const Math::Vec4& v4) :
        type(Type::Vec4), value(v4) {}

    JsonValue(const ArrayType& arr) :
        type(Type::Array), value(arr) {}
    JsonValue(const ObjectType& obj) :
        type(Type::Object), value(obj) {}

    bool parseStringColorWithName(const JsonType& node, const std::string& valueName);
    bool parseWithName(const JsonType& node, const std::string& valueName);

    void parse(const JsonType& j);

    template <typename T>
    bool is() const { return std::holds_alternative<T>(value); }

    template <typename T>
    T& get() { return std::get<T>(value); }

    template <typename T>
    const T& get() const { return std::get<T>(value); }
};
} // namespace Desc
} // namespace Voxol::Scene
#endif