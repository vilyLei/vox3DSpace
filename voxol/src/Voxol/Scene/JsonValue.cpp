#include "JsonValue.h"

namespace Voxol::Scene
{
namespace Data
{
bool ColorValue::parsePantone(const std::string& s){
    return false;
};

bool ColorValue::parse(const JsonType& node)
{
    return parseWithName(node, "color") ||
        parseWithName(node, "fillColor") ||
        parseWithName(node, "strokeColor");
}

bool ColorValue::parseWithName(const JsonType& node, const std::string& key)
{
    if (!node.contains(key))
        return false;

    const auto& v = node[key];

    if (v.is_number())
    {
        value = static_cast<uint32_t>(v);
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

// ----------------------------------------
// parse ["r","g","b"] or ["r","g","b","a"]
// float[0-1],uint8[0-255]
// ----------------------------------------
bool ColorValue::parseColorArray(const JsonType& arr)
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

    value = (A << 24) | (R << 16) | (G << 8) | B;
    return true;
}

// ----------------------------------------
// parse string "#RRGGBB", "0xff00ff", "ff00ff", "red"
// ----------------------------------------
bool ColorValue::parseColorString(std::string s)
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

bool ColorValue::isHexString(const std::string& s)
{
    return std::all_of(s.begin(), s.end(), [](char c) {
        return std::isxdigit(c);
    });
}

// ----------------------------------------
// RRGGBB / RRGGBBAA
// ----------------------------------------
bool ColorValue::parseHex(const std::string& hex)
{
    uint32_t v = std::stoul(hex, nullptr, 16);

    if (hex.size() == 6)
    {
        // RGB ¡ú ARGB
        value = 0xFF000000 | v;
    }
    else if (hex.size() == 8)
    {
        // RGBA ¡ú ARGB
        //uint32_t r = (v >> 24) & 0xFF;
        //uint32_t g = (v >> 16) & 0xFF;
        //uint32_t b = (v >> 8) & 0xFF;
        //uint32_t a = (v >> 0) & 0xFF;
        //argb      = (a << 24) | (r << 16) | (g << 8) | b;
        value = v;
    }
    else
    {
        return false;
    }
    return true;
}

bool ColorValue::parseCssName(const std::string& name)
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
        {"purple", 0xFF800080},
        {"gray", 0xFF808080},
        {"grey", 0xFF808080},
    };

    auto it = cssColors.find(name);
    if (it != cssColors.end())
    {
        value = it->second;
        return true;
    }
    return false;
}

bool JsonValue::parseStringColorWithName(const JsonType& node, const std::string& valueName)
{
    if (!node.contains(valueName))
        return false;
    uint32_t color = 0x0;
    auto&& vo = node[valueName];
    if (vo.is_number())
    {
        color = static_cast<uint32_t>(vo);
    }
    else if (vo.is_string())
    {
        std::string&& hex_str = vo;
        std::transform(hex_str.begin(), hex_str.end(), hex_str.begin(),
                       [](unsigned char c) { return std::tolower(c); });

        if (hex_str.find('#') == 0)
        {
            color = std::stoul(hex_str.substr(1), nullptr, 16);
        }
        else if (hex_str.find('x') == 1)
        {
            if (hex_str.size() >= 3)
            {
                color = std::stoul(hex_str.substr(2), nullptr, 16);
            }
        }
        else
        {
            color = std::stoul(hex_str.substr(2), nullptr, 16);
        }
    }
    printf("JsonValue::parseStringColorWithName(), color: %x\n", color);
    value = color;
    return true;
}
bool JsonValue::parseWithName(const JsonType& node, const std::string& valueName)
{
    if (!node.contains(valueName))
        return false;

    parse(node[valueName]);
    return true;
}
void JsonValue::parse(const JsonType& j)
{
    if (j.is_null())
    {
        type  = Type::Null;
        value = std::monostate{};
    }
    else if (j.is_boolean())
    {
        type  = Type::Bool;
        value = static_cast<bool>(j);
    }
    else if (j.is_number_float())
    {
        type  = Type::Float;
        value = static_cast<float>(j);
    }
    else if (j.is_number_integer())
    {
        type  = Type::Int;
        value = static_cast<int>(j);
    }
    else if (j.is_number_unsigned())
    {
        type  = Type::Uint32;
        value = static_cast<uint32_t>(j);
    }
    else if (j.is_string())
    {
        type  = Type::String;
        value = j.get<std::string>();
    }
    else if (j.is_array())
    {
        // vec2/vec3/vec4 auto-detect
        if (j.size() == 2 && j[0].is_number() && j[1].is_number())
        {
            type  = Type::Vec2;
            value = Math::Vec2{(float)j[0], (float)j[1]};
            return;
        }
        if (j.size() == 3)
        {
            bool allNum = true;
            for (auto& e : j)
                if (!e.is_number()) allNum = false;
            if (allNum)
            {
                type  = Type::Vec3;
                value = Math::Vec3{(float)j[0], (float)j[1], (float)j[2]};
                return;
            }
        }
        if (j.size() == 4)
        {
            bool allNum = true;
            for (auto& e : j)
                if (!e.is_number()) allNum = false;
            if (allNum)
            {
                type  = Type::Vec4;
                value = Math::Vec4{
                    (float)j[0], (float)j[1],
                    (float)j[2], (float)j[3]};
                return;
            }
        }

        // general array
        type = Type::Array;
        {
            ArrayType arr;
            arr.reserve(j.size());
            for (auto& item : j)
            {
                JsonValue val;
                val.parse(item);
                arr.push_back(std::move(val));
            }
            value = std::move(arr);
        }
    }
    else if (j.is_object())
    {
        type = Type::Object;
        ObjectType obj;
        for (auto it = j.begin(); it != j.end(); ++it)
        {
            JsonValue val;
            val.parse(it.value());
            obj[it.key()] = std::move(val);
        }
        value = std::move(obj);
    }
}
}
} // namespace Voxol::Scene