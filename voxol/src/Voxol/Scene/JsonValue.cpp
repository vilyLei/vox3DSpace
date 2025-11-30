#include "JsonValue.h"

namespace Voxol::Scene
{
namespace Data
{
bool ColorValue::parsePantone(const std::string& s)
{
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
        color.value = static_cast<uint32_t>(v);
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

    color.value = (A << 24) | (R << 16) | (G << 8) | B;
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

    if (hex.size() <= 6)
    {
        // RGB ¡ú ARGB
        color.value = 0xFF000000 | v;
        return true;
    }

    if (hex.size() == 8)
    {
        color.value = v;
        return true;
    }

    return false;
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
        color.value = it->second;
        return true;
    }
    return false;
}

bool JsonValue::parseColor(const JsonType& node)
{
    ColorValue cv;
    if (!cv.parse(node))
    {
        return false;
    }
    value = cv.color.argb();
    printf("JsonValue::parseColor(), color: %s\n", cv.color.toHexString().c_str());
    return true;
}

bool JsonValue::parseTextWithName(const JsonType& node, const std::string& valueName)
{

    if (!node.contains(valueName))
        return false;

    auto&& jNode = node[valueName];

    Component::UnitTextDesc textDesc;

    if (node.contains("content"))
    {

        std::string str = node["content"];
        if (str.empty())
            return false;

        textDesc.text = str;
    }
    if (node.contains("fontSize") && node["fontSize"].is_number())
    {
        auto v            = static_cast<float>(node["fontSize"]);
        textDesc.fontSize = std::isnan(v) ? 0.0f : v;
    }
    if (jNode.contains("color"))
    {
        ColorValue cv;
        cv.parse(jNode["color"]);
        textDesc.color = cv.color.argb();
    }
    value = textDesc;
    return true;
}
bool JsonValue::parseColorWithName(const JsonType& node, const std::string& valueName)
{

    if (!node.contains(valueName))
        return false;

    ColorValue cv;
    if (!cv.parseWithName(node, valueName))
    {
        return false;
    }
    value = cv.color.argb();
    printf("JsonValue::parseStringColorWithName(), color: %s\n", cv.color.toHexString().c_str());
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
float JsonValue::getFloat()
{
    if (is<float>())
    {
        return get<float>();
    }
    else if (is<int>())
    {
        return get<int>();
    }
}
} // namespace Data
} // namespace Voxol::Scene